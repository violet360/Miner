# efficientMining
An Efficient Miner Strategy for Selecting Cryptocurrency Transactions to form a valid block under given constraints

## summary of my approach:
* seems like a 0-1 knapsack with some differences 
* for every elemnt you consider to put in the kna[sack check whether its immediate parent is in it 
* but in this dataset if knapsack is applied, space and time complexity will skyrocket, clearly not applicable for practical purposes
* a more feasible approach is to not get stuck with the most optimal answer, instead going for the good enough answer
* view the mempool as a forest of directed graphs
* size of each component ranging from 1 to some positive integer
* topologically sort each component
* each component will have a total fee and weight, sort all components on the basis of their fees and weight ratio (fees/weight) in descending order
* greedily pick the components from the largest ratio to the smallest as long as the threshold weight is not crossed 
* when threshold is crossed try to fill in the remaining gap by smaller ratios from the end of the sorted array
* and at last you will get a good enough topologically sorted block

## why not 0-1 knapsack?
If we were only asked to find the maximum fees, without any contraints of parent we could have applied that, and space would have reduced to O(nW) and with a bit more
optimization it will further reduce to O(W), where W is the maximum weight, but here we have to track parents , theoretically a recursive function could be developed but
the space and time complexity will skyrocket
why?
let us try the knapsack approach
* sort the mempool topologically first, since we will be moving from left to right and this will give us a surety that if we are at a transaction we have scanned it's parents
in the knapsack, so we can check wheather it was included or not 
* P<sub>i</sub> be a set of parents of ith transaction and added[] is a hashmap which tells whether an element is added in the knapsack or not
* node(i, W<sub>curr</sub>) = <br>
&nbsp;&nbsp;if basecase: <br>
&nbsp;&nbsp;&nbsp;&nbsp;return 0; <br>
&nbsp;&nbsp;allPresent = 1;<br>
&nbsp;&nbsp;if(P<sub>i</sub>.empty): <br>
&nbsp;&nbsp;&nbsp;&nbsp;return fees[i] + node(i+1, W<sub>curr</sub> + w<sub>i</sub>, added)
&nbsp;&nbsp;for p in P<sub>i</sub>:<br>
&nbsp;&nbsp;&nbsp;&nbsp;allPresent *= added[p]<br>
&nbsp;&nbsp;if(allPresent):<br>
&nbsp;&nbsp;&nbsp;&nbsp;added[i] = true;<br>
&nbsp;&nbsp;&nbsp;&nbsp;return max{fees<sub>i</sub> + node(i+1, W<sub>curr</sub> + w<sub>i</sub>, added), node(i+1, W<sub>curr</sub>, added)}<br>
&nbsp;&nbsp;else if(not allPresent):<br>
&nbsp;&nbsp;&nbsp;&nbsp;return node(i+1, W<sub>curr</sub>, added);<br>
* at every recursive call added is copied to the stack 
* we cannot memoization won't help reduce it
* so both space and time complexity will be in the order of 2<sup>n</sup> which ain't feasible

## can we do better than the original solution ?
Quality of answer can be improved , we can apply knapsack at vector<vector<ll>>collection, this will reap more fees for us, and it will be topologically sorted but, this will
come at a cost of time complexity O(mW) where m is nomber of components or collection.length, space complexity will be O(W)

## some other method ?
[size density table](https://www.researchgate.net/publication/337199653_An_Efficient_Miner_Strategy_for_Selecting_Cryptocurrency_Transactions) looks like a promising method, we can apply this algorithm to collection vector, have requested the full text, once I have it, I will implement it