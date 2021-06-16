/*
* author : Shashank Karmakar
* some useful links to take reference
* https://www.geeksforgeeks.org/topological-sorting/
* https://cp-algorithms.com/graph/topological-sort.html

* summary of my approach:
* seems like a 0-1 knapsack with some differences 
* for every elemnt you consider to put in the kna[sack check whether its immediate parent is in it 
* but in this dataset if knapsack is applied, space and time complexity will skyrocket, clearly not applicable for practical purposes
* a more feasible approach is to not get stuck with the most optimal answer, instead going for the good enough answer
* view the mempool as a forest of directed graphs
* size of each component ranging from 1 to some positive integer
* topologically sort each component
* each component will have a total fee and weight, sort all components on the basis of their fees and weight ratio (fees/weight) in descending 
* greedily pick the components from the largest ratio to the smallest as long as the threshold weight is not crossed 
* when threshold is crossed try to fill in the remaining gap by smaller ratios from the end
* and at last you will get a good enough topologically sorted block
*/



#include <bits/stdc++.h>
typedef long long ll;
typedef long double ld;
# define pb push_back
# define FAST_CODE ios_base::sync_with_stdio(false); cin.tie(NULL);
using namespace std;


struct package {
    unordered_map<ll, vector<ll>>unDiGraph;
    unordered_map<ll, vector<ll>>trackParent;
    unordered_map<ll, vector<ll>>diGraph;
    unordered_map<ll, string>numToHash;
    unordered_map<string, ll>hashToNum;
    unordered_map<ll, ll>weight;
    unordered_map<ll, ll>fee;
};


struct txSet {
    ll idx, weight, fees;
 
    // Constructor
    txSet(ll idx, ll fees, ll weight)
    {
       this->fees=fees;
       this->weight=weight;
       this->idx = idx;
    }
};
/*
reads the csv file and wraps up data in graphs and hashtables
*/
package read() {
    ifstream fin;
    string line;
    string line1;
    // Open an existing file
    fin.open("mempool.csv");
    unordered_map<ll, vector<ll>>unDiGraph;
    unordered_map<ll, vector<ll>>graph;
    unordered_map<ll, string>numToHash;
    unordered_map<string, ll>hashToNum;
    unordered_map<ll, ll>weight;
    unordered_map<ll, ll>fee;
    unordered_map<ll, vector<ll>>trackParent;
    ll i = 1;
     while(!fin.eof()) {
                fin>>line;
                if(i==1) {
                    i++;
                    continue;
                }
                stringstream  ss(line);
                string str;
                vector<string>columns;
                while (getline(ss, str, ',')) {
                    columns.push_back(str);
                }

                numToHash[i] = columns[0];
                hashToNum[columns[0]] = i;
                fee[i] = stoll(columns[1], nullptr, 10);
                weight[i] = stoll(columns[2], nullptr, 10);
                vector<ll>temp;
                trackParent[i] = temp;
                graph[i] = temp;
                unDiGraph[i] = temp;
                i++;
        }

        fin.close();
        fin.open("mempool.csv");

        i=1;
        while(!fin.eof()) {
                fin>>line1;
                if(i==1) {
                    i++;
                    continue;
                }
                stringstream  ss(line1);
                string str;
                vector<string>columns;
                while (getline(ss, str, ',')) {
                    columns.push_back(str);
                }

                if(columns.size() > 3) {
                    string parentString = columns[3];
                    stringstream stream(parentString);
                    string parent;
                    while(getline(stream, parent, ';')) {
                        trackParent[i].push_back(hashToNum[parent]);
                        graph[hashToNum[parent]].push_back(i);
                        unDiGraph[hashToNum[parent]].push_back(i);
                        unDiGraph[i].push_back(hashToNum[parent]);
                    }
                }
                i++;
        }
        fin.close();

    return {unDiGraph, trackParent, graph, numToHash, hashToNum, weight, fee};
}
/*
*utility function to topologically sort a graph
*@param current -> current node
*@param adj-> adjacency matrix
*@param visited[] -> hashtable to track visited and unvisited nodes
*/
void topoSortUtil(ll current, unordered_map<ll, vector<ll>>&adj, unordered_map<ll, bool>&visited, stack<ll>&s) {
	visited[current] = true;
	for(ll child:adj[current])
	{
		if(!visited[child])
            topoSortUtil(child, adj, visited, s);
	}
	s.push(current);
}

stack<ll> topoSort(vector<ll>&nodes, unordered_map<ll, vector<ll>>&adj) {
    ll n = nodes.size(); //5214 nodes
    unordered_map<ll, bool>visited;

    for(ll n:nodes) {
        visited[n] = false;
    }
    stack<ll>s;

	for(ll n : nodes) { //we need to iterate till 5215 and n is 5214
		if(!visited[n])
			topoSortUtil(n, adj, visited, s);
	}
    return s;
}

vector<ll> getOrder(vector<ll>&nodes, unordered_map<ll, vector<ll>>&adj) {
    stack<ll>s = topoSort(nodes, adj);
    vector<ll>topOrder;
    while(!s.empty()) {
        topOrder.push_back(s.top());
        s.pop();
    }
    return topOrder;
}

vector<vector<ll>>collectTopoSortedComponent(vector<vector<ll>>collect, unordered_map<ll, vector<ll>>&diGraph) {
    vector<vector<ll>>result;
    for(auto com: collect) {
        result.push_back(getOrder(com, diGraph));   
    }
    return result;
}

void dfs(ll node, vector<ll>&component, vector<ll>&visited, unordered_map<ll, vector<ll>>&unDiGraph) {
    visited[node] = true;
    component.push_back(node);
    for(ll u:unDiGraph[node]) {
        if(!visited[u]) {
            dfs(u, component, visited, unDiGraph);
        }
    }   
}

vector<vector<ll>> components(unordered_map<ll, vector<ll>>&unDiGraph) {
    ll n = unDiGraph.size();
    vector<ll>visited(n+2, 0);
    vector<vector<ll>>collectComponents;
    for(ll i=2; i<=n+1; ++i) {
        if(!visited[i]) {
            vector<ll>component;
            dfs(i, component, visited, unDiGraph);
            collectComponents.push_back(component);
        }
    }
    return collectComponents;
}

vector<txSet> getTransactionSets(vector<vector<ll>>&vec, unordered_map<ll, ll>&weight, unordered_map<ll, ll>&fee) {
    vector<txSet>transactionSets;
    for(ll i=0; i<vec.size(); ++i) {
        ll W = 0;
        ll F = 0;
        for(ll c : vec[i]) {
            W += weight[c];
            F += fee[c];
        }
        transactionSets.push_back({i, F, W});
    }
    return transactionSets;
}

bool cmp(struct txSet a, struct txSet b) {
    ld r1 = (ld)a.fees / (ld)a.weight;
    ld r2 = (ld)b.fees / (ld)b.weight;
    return r1 > r2;
}


pair<ld, vector<ll>> generateBlock(ll W, vector<txSet>&t) {
    ll curWeight = 0; // Current weight in knapsack
    ld finalvalue = 0.0; // Result (value in Knapsack)
    ll n = t.size();
    ll i=0;
    vector<ll>ValidTransactions;
    vector<ll>extraAddOn;
        // If adding Item won't overflow, add it completely
    while (curWeight + t[i].weight <= W) {
            curWeight += t[i].weight;
            finalvalue += t[i].fees;
            ValidTransactions.push_back(i);
            i++;
    }

    ll fromR = n-1;

    while(curWeight + t[fromR].weight <= W) {
        curWeight += t[fromR].weight;
        finalvalue += t[fromR].fees;
        ValidTransactions.push_back(fromR);
        fromR--;
    }

    
 
    // Returning final value
    return {finalvalue, ValidTransactions};
}

int main() {
	FAST_CODE;
    #ifndef ONLINE_JUDGE
		freopen("block.txt","w", stdout);
    #endif
    package p;
    p = read(); //this reads the csv file and arranges the represents the data in important data structures, graph and hashtable here
    unordered_map<ll, vector<ll>>unDiGraph = p.unDiGraph; //unidirectional graph adjacency list of the mempool elements
    unordered_map<ll, vector<ll>>diGraph = p.diGraph; //directional graph adjacency list
    unordered_map<ll, string>numToHash = p.numToHash;//if x is a txId in ith line then it's hashToNum value is i, through the solution the txId is represented by the line in which it resides
    // example numToHash["2e3da8fbc1eaca8ed9b7c2db9e6545d8ccac3c67deadee95db050e41c1eedfc0"] = 2;
    unordered_map<string, ll>hashToNum = p.hashToNum; // reverse of the above
    unordered_map<ll, vector<ll>>trackParent = p.trackParent; // if i = numToHash[txid]; then trackparent[i] is vector of parents of txid 
    unordered_map<ll, ll>weight = p.weight; // if i = numToHash[txid]; then weight[i] is weight of txid
    unordered_map<ll, ll>fee = p.fee; // if i = numToHash[txid]; then fee[i] is fee of txid
    vector<vector<ll>>collectComponents = components(unDiGraph); // list of components in the forest
    vector<vector<ll>>collection = collectTopoSortedComponent(collectComponents, diGraph); // same as vector collectComponents but each compoenent is topologically sorted
    vector<txSet>transactionSets = getTransactionSets(collection, weight, fee); // list of {weight, fees, idx} of each component where idx is the index of the component in collection
    sort(transactionSets.begin(), transactionSets.end(), cmp); // list components sorted by fees/weight 
    pair<ld, vector<ll>>validBlockInfo = generateBlock(ll(4000000), transactionSets); // greedily select components for the the block on the basis of the ratio, knida like fractional knapsack
    vector<ll>aValidBlock = validBlockInfo.second; // if 'e 'is any element in aValid block, then transactionSets[u] gives [idx, weight, fee] of that component where idx is the index in "collection"
    //weight and fee are the combined weight and fee of the component collection[idx] 
    ld maxFees = validBlockInfo.first;

    for(ll u: aValidBlock) {
        ll temp  = transactionSets[u].idx;
        for(ll u : collection[temp]) {
            cout<<numToHash[u]<<"\n";
        }
    }
	return 0;
}
