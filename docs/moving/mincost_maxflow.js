
class Edge {
    constructor(to, cap, cost) {
        this.to = to;
        this.rev = undefined;
        this.cap = cap;
        this.cost = cost;
    }
}

class Node {
    constructor(id) {
        this.id = id;
        this.es = new Array();
    }
    addEdge(to, cap, cost) {
        let e = new Edge(to, cap, cost);
        let rev = new Edge(this, 0, -cost);
        e.rev = rev;
        rev.rev = e;
        this.es.push(e);
        to.es.push(rev);
    }
}

/**
 * sからtへの最大流でコスト最小を求める。
 * @param {Array<Node>} g - 全ての頂点を含む配列。
 * @param {Node} s - 始点。
 * @param {Node} t - 終点。
 * @return {Array} 長さ 2 の配列を返す。 [コスト, 流量] を意味する。
 */
function mincost_maxflow(g, s, t) {
    const INF = 1e18;
    const EPS = 1e-8;
    const N = g.length;
    let result_cost = 0, result_flow = 0;
    for (;;) {
        for (let v of g) {
            v.dist = INF;
            v.prev = undefined;
        }
        s.dist = 0;
        for (let iter = 0; iter < N; iter++) {
            let update = false;
            for (let v of g) if (v.dist < INF / 2) {
                for (let e of v.es) if (e.cap > 0) {
                    if (e.to.dist > v.dist + e.cost + EPS) {
                        e.to.dist = v.dist + e.cost + EPS;
                        e.to.prev = e;
                        update = true;
                    }
                }
            }
            if (!update) break;
        }
        if (t.dist >= INF/2) break;

        flow = INF;
        let i = 0;
        for (let e = t.prev; e ; e = e.rev.to.prev) {
            if (flow > e.cap) flow = e.cap;
        }
        result_flow += flow;
        for (let e = t.prev; e ; e = e.rev.to.prev) {
            result_cost += flow * e.cost;
            e.cap -= flow;
            e.rev.cap += flow;
        }
    }
    return [result_cost, result_flow];
}

function print_cap(g) {
    for (v of g) {
        for (e of v.es) {
            console.log(v.id, "->", e.to.id, e.cap, e.cost);
        }
    }
}

// let g = [];
// for (let i = 0; i < 4; i++) {
//     g.push(new Node("v"+i));
// }
// g[0].addEdge(g[1], 2, 1);
// g[0].addEdge(g[2], 1, 2);
// g[1].addEdge(g[2], 1, 1);
// g[1].addEdge(g[3], 1, 3);
// g[2].addEdge(g[3], 2, 1);
// let result = mincost_maxflow(g, g[0], g[3]);
// print_cap(g);

// let a = [
//     [10, 4, 10],
//     [1, 15, 3],
//     [4, 5, 8],
// ];

// let g = [];
// for (let i = 0; i < 3; i++) g.push(new Node("L" + i));
// for (let i = 0; i < 3; i++) g.push(new Node("R" + i));
// let s = new Node("s");
// let t = new Node("t");
// g.push(s);
// g.push(t);
// for (let i = 0; i < 3; i++) {
//     for (let j = 0; j < 3; j++) {
//         g[i].addEdge(g[3+j], 1, a[i][j]);
//     }
// }
// for (let i = 0; i < 3; i++) s.addEdge(g[i], 1, 0);
// for (let j = 0; j < 3; j++) g[3+j].addEdge(t, 1, 0);
// let result = mincost_maxflow(g, s, t);
// print_cap(g);
// console.log(result);

