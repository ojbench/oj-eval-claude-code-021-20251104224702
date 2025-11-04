#include <bits/stdc++.h>
using namespace std;

/*
Heuristic controller without simulator.
Variant v5: coprime-target sweeps + micro-oscillations.
 - Deterministic seeding by input and grid so behavior is fixed per test
 - Sweep between ±v where v are chosen from a coprime-ish set to
   diversify angles (e.g., primes) scaled by n
 - Between sweeps, insert micro oscillations (EA / DB pairs) to
   decorrelate paths without drifting vx
*/

static uint64_t splitmix64(uint64_t x){
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
struct RNG{
    uint64_t s; explicit RNG(uint64_t seed):s(seed){}
    uint64_t next(){ return s = splitmix64(s); }
    int uniform_int(int l,int r){ if(l>r) swap(l,r); return l + (int)(next()% (uint64_t)(r-l+1)); }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k; if(!(cin>>k)) return 0;
    int n; cin>>n; long long m; cin>>m; int s; cin>>s;
    int N = 2*n;
    // Seed by grid
    uint64_t h = 1469598103934665603ULL, P=1099511628211ULL;
    for(int i=0;i<N;i++) for(int j=0;j<N;j++){ int c; cin>>c; h^=(uint64_t)(c+3)*(i+1)*(j+7); h*=P; }
    uint64_t seed = ((uint64_t)k<<48) ^ ((uint64_t)n<<32) ^ (uint64_t)(m*1315423911u) ^ ((uint64_t)s<<5) ^ h ^ 0x9E3779B97F4A7C15ULL;
    RNG rng(seed);

    auto emit=[&](char ch){ cout<<ch<<'\n'; };
    long long t=m, cnt=0; int vx=0;

    auto accel_to = [&](int target){
        while(cnt<t && vx<target){ int d=min(2,target-vx); emit(d==2?'E':'D'); vx+=d; ++cnt; }
        while(cnt<t && vx>target){ int d=min(2,vx-target); emit(d==2?'A':'B'); vx-=d; ++cnt; }
    };
    auto hold = [&](int H){ for(int i=0;i<H && cnt<t;i++){ emit('C'); ++cnt; } };
    auto micro = [&](int repeats){ // zero-drift jitter
        for(int i=0;i<repeats && cnt+1<t;i++){
            if(rng.uniform_int(0,1)==0){ emit('E'); emit('A'); }
            else { emit('D'); emit('B'); }
            cnt+=2;
        }
    };

    // Build target speed set from primes scaled by n
    int scale = max(1, n/6);
    vector<int> primes = {3,5,7,11,13,17,19};
    vector<int> targets;
    for(int p:primes){ long long v=1LL*p*scale; if(v<3) v=3; if(v> (long long)max(20, n)) v = max(20, n); targets.push_back((int)v); }
    // Shuffle order deterministically
    for(int i=targets.size()-1;i>0;i--){ int j=rng.uniform_int(0,i); swap(targets[i],targets[j]); }

    // Warm start
    accel_to(min(targets[0], max(4, n/4)));
    hold(max(4, n*2));

    // Main schedule: cycle through targets
    size_t idx=0;
    while(cnt<t){
        int V = targets[idx++ % targets.size()];
        // Right plateau
        accel_to(+V); hold(max(n, V*3)); micro(rng.uniform_int(1, max(2, n/6)));
        if(cnt>=t) break;
        // Left plateau
        accel_to(-V); hold(max(n, V*3)); micro(rng.uniform_int(1, max(2, n/6)));
        // Mid-cycle neutral hold occasionally
        if(rng.uniform_int(0,3)==0){ accel_to(0); hold(max(3, n)); }
    }

    // Fill remainder with neutral
    while(cnt<t){ emit('C'); ++cnt; }
    return 0;
}
