#include <iostream>
#include <vector>
#include <algorithm>

class Dijkstra
{
public:
	struct Edge
	{
		Edge(size_t p, long long pr) : ID(p), tripPrice(pr) {};
		size_t ID;
		long long tripPrice;
	};
	struct Vertex
	{
		Vertex() : ID(0), tripPrice(-1), poprzednik(nullptr),krawedz() {};
		Vertex(size_t ID, long long tp, Vertex* p) : ID(ID), tripPrice(tp), poprzednik(p) {};
		size_t ID;
		long long tripPrice;
		Vertex* poprzednik;
		std::vector<Edge> krawedz;
	};
public:
	Dijkstra() = delete;
	static long long CalculateFor(size_t start, size_t end)
	{
		//Je¿eli nic nie ma w Q
		if (Q.size() == 0)
		{
			Q.clear();
			if (S.size() == 0) return -1;
			Q.reserve(S.size());
			//To skopiuj z S
			for (size_t i = 0; i < S.size(); ++i)
			{
				Q.push_back(S[i]);
			}
		}

		//Wyczyœæ wszystko
		S.clear();
		S.reserve(Q.size());
		for (size_t i = 0; i < Q.size(); ++i)
		{
			Q[i].poprzednik = nullptr;
			if (Q[i].ID!=start)Q[i].tripPrice = MAX_INT;
			else Q[i].tripPrice = 0;
		}

		while (Q.size() > 0)
		{
			//Znajdz wierzcho³ek o najmniejszej drodze dojscia
			size_t minID = 0;
			long long min = Q[0].tripPrice;
			for (size_t i = 1; i < Q.size(); ++i)
			{
				if (Q[i].tripPrice < (long long)min)
				{
					min = Q[i].tripPrice;
					minID = i;
				}
			}

			//Wyrzuc z Q dodaj do S
			S.push_back(Q[minID]);
			Q[minID] = Q[Q.size() - 1];
			Q.pop_back();

			Vertex* current = &S[S.size() - 1];
			//Dla ka¿dego s¹siada 
			for (size_t i = 0; i < current->krawedz.size(); ++i)
			{
				//Który jest w zbiorze Q
				Vertex* szukany = exist(current->krawedz[i].ID, Q);
				if (szukany != nullptr)
				{
					if (szukany->tripPrice > current->tripPrice + current->krawedz[i].tripPrice)
					{
						szukany->tripPrice = current->tripPrice + current->krawedz[i].tripPrice;
						szukany->poprzednik = current;
					}
				}
			}
		}

		for (size_t i = 0, e = S.size(); i < e; ++i)
		{
			if (S[i].ID == end) return S[i].tripPrice;
		}
		return -1;
	}
	static void loadVertex(size_t ID)
	{
		Q.emplace_back(ID, MAX_INT, nullptr);
	};
	static void loadEdge(size_t ID, size_t IDDest, long long cost) 
	{
		for (size_t i = 0, e = Q.size(); i < e; ++i)
		{
			if (Q[i].ID == ID)
			{
				Q[i].krawedz.emplace_back(IDDest, cost);
				break;
			}
		}
	};
	static std::vector<Vertex> getSolution() { return S; };
private:
	static Vertex* exist(size_t ID, std::vector<Vertex>&Q)
	{
		for (size_t i = 0; i < Q.size(); ++i)
		{
			if (ID == Q[i].ID) return &Q[i];
		}
		return nullptr;
	}
private:
	static std::vector<Vertex> Q;
	static std::vector<Vertex> S;
public:
	static const long long MAX_INT;
	static size_t nVertex;
	static size_t nEdges;
};
std::vector<Dijkstra::Vertex> Dijkstra::Q;
std::vector<Dijkstra::Vertex> Dijkstra::S;
size_t Dijkstra::nVertex = 0;
size_t Dijkstra::nEdges = 0;
const long long Dijkstra::MAX_INT = 4000000000000000000;
class Alchemia
{
public:
	Alchemia()
	{
		std::cin >> nMetale;
		Dijkstra::nVertex = nMetale;
		metale.reserve(nMetale);
		long long price;
		for (size_t j = 0; j < nMetale; ++j)
		{
			std::cin >> price;
			metale.emplace_back(price, j + 1);
			Dijkstra::loadVertex(j + 1);
		}

		std::cin >> nRecipe;
		Dijkstra::nEdges = nRecipe;
		size_t c1, c2;
		for (size_t j = 0; j < nRecipe; ++j)
		{
			std::cin >> c1 >> c2 >> price;
			Dijkstra::loadEdge(c1, c2, (long long)price);
		}
	};

	long long minPrice()
	{
		calculateprices();
		return Min;
	}
private:
	struct Metale
	{
		Metale(long long price, size_t id) : price(price), ID(id) {};
		long long price;
		size_t ID;
	};
	void calculateprices()
	{
		//Sort all 
		std::sort(metale.begin(), metale.end(), [](const Metale& m1,const Metale& m2) { return m1.ID < m2.ID; });


		//Czy zloto nie najtansze
		if (theCheapest(1))
		{
			Min = long long(0.5*metale[0].price);
			return;
		}

		size_t e = Dijkstra::nVertex;
		long long min = Dijkstra::MAX_INT;

		// z 1 gdziekolwiek
		Dijkstra::CalculateFor(1, e);
		std::vector<Dijkstra::Vertex> OneToX = Dijkstra::getSolution();

		//z X do 1
		std::vector<Dijkstra::Vertex> XtoOne;
		for (size_t i = 0; i < e; ++i)
		{
			if (i == 0)	XtoOne.emplace_back(i + 1, Dijkstra::MAX_INT, nullptr);
			else XtoOne.emplace_back(i+1,Dijkstra::CalculateFor(i+1, 1) + (long long)(0.5*metale[i].price),nullptr);
		}

		std::sort(OneToX.begin(), OneToX.end(), [](const Dijkstra::Vertex& v1, const Dijkstra::Vertex& v2) {return v1.ID < v2.ID; });
		Min = XtoOne[0].tripPrice + OneToX[0].tripPrice;
		for (size_t i = 1, e2 = OneToX.size(); i < e2; ++i)
		{
			long long tmp = XtoOne[i].tripPrice + OneToX[i].tripPrice;
			if (tmp < Min) Min = tmp;
		}
		if (Min > 0.5*metale[0].price) Min = 0.5*metale[0].price;
	}
	bool theCheapest(size_t id)
	{
		long long min = Dijkstra::MAX_INT;
		for (const Metale& m : metale)
		{
			if (m.price < min) min = m.price;
		}
		return metale[id-1].price <= min;
	}
public:
	size_t nMetale;
	size_t nRecipe;
	std::vector<Metale> metale;
	long long Min;
};


int main()
{
	size_t nCase;
	std::cin >> nCase;
	for (size_t i = 0; i < nCase; ++i)
	{
		Alchemia a;
		std::cout<<a.minPrice()<<'\n';
	}
	return 0;
}