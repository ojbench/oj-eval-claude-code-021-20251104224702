#include <bits/stdc++.h>
using namespace std;

/*
Heuristic controller without simulator.
Variant v4: seeded pseudo-random multi-segment sweeps.
 - Deterministic RNG seeded by input (k,n,m,s and color grid hash)
 - Alternate/randomize target horizontal speeds and hold lengths
 - Insert occasional resets and decorrelation nudges
Goal: diversify trajectories per testcase to raise expected reward.
*/

static uint64_t splitmix64(uint64_t x){
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

struct RNG {
    uint64_t s;
    explicit RNG(uint64_t seed): s(seed){}
    uint64_t next(){ return s = splitmix64(s); }
    int uniform_int(int l,int r){ // inclusive
        if(l>r) swap(l,r);
        return l + (int)(next() % (uint64_t)(r - l + 1));
    }
    double uniform(){ return (next() >> 11) * (1.0/9007199254740992.0); }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k; if(!(cin>>k)) return 0;
    int n; cin>>n;
    long long m; cin>>m;
    int s; cin>>s;
    int N = 2*n;
    // Hash grid to seed RNG
    uint64_t h = 1469598103934665603ULL; // FNV offset basis
    const uint64_t FNV_PRIME = 1099511628211ULL;
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            int c; cin>>c;
            h ^= (uint64_t)(c + 1) * 1315423911u + (uint64_t)(((uint64_t)i<<21) ^ ((uint64_t)j<<7));
            h *= FNV_PRIME;
        }
    }
    uint64_t seed = ((uint64_t)k<<48) ^ ((uint64_t)n<<32) ^ ((uint64_t)(m&0xffffffff)<<1) ^ ((uint64_t)(s&0xffff)<<17) ^ h ^ 0xA5A5A5A5DEADBEEFULL;
    RNG rng(seed);

    long long t = m;
    auto emit = [&](char ch){ cout<<ch<<'\n'; };

    long long cnt = 0;
    int vx = 0; // intended horizontal speed we track

    auto accel_to = [&](int target){
        while(cnt < t && vx < target){
            int step = min(2, target - vx);
            emit(step==2?'E':'D'); vx += step; ++cnt;
        }
        while(cnt < t && vx > target){
            int step = min(2, vx - target);
            emit(step==2?'A':'B'); vx -= step; ++cnt;
        }
    };
    auto hold = [&](int c){ for(int i=0;i<c && cnt<t;i++){ emit('C'); ++cnt; } };
    auto nudge = [&](int dir, int times){ // dir: +1 right, -1 left
        for(int i=0;i<times && cnt<t;i++){
            if(dir>0){ emit('D'); vx += 1; }
            else { emit('B'); vx -= 1; }
            ++cnt;
        }
    };
    auto clip_vx = [&](int cap){ if(vx>cap) accel_to(cap); else if(vx<-cap) accel_to(-cap); };

    // Parameter ranges based on n
    int baseV = max(6, n/3);
    int vmax = max(baseV+4, n/2);
    int hardCap = max(vmax*3, 20);
    int holdMin = max(6, n*2);
    int holdMax = max(holdMin+4, n*10);

    // Warm start: light right bias and short hold
    accel_to(min(baseV, 6));
    hold(holdMin/2);

    // Main loop: generate segments until t operations
    while(cnt < t){
        // Choose a direction and magnitude
        int mag = rng.uniform_int(max(3, baseV/2), vmax);
        int dir = (rng.uniform() < 0.5 ? +1 : -1);
        // Occasionally force alternation to avoid drift
        if(rng.uniform() < 0.3){ dir = (vx >= 0 ? -1 : +1); }
        int target = dir * mag;
        // Accelerate to target and hold
        accel_to(target);
        int H = rng.uniform_int(holdMin, holdMax);
        hold(H);

        if(cnt >= t) break;

        // Decorrelation: brief counter nudges and partial reset
        if(rng.uniform() < 0.6){
            int nud = max(1, mag/8);
            nudge(-dir, nud);
        }
        if(rng.uniform() < 0.25){
            // Soft reset towards 0 velocity
            int delta = vx/2; // move halfway to zero
            int target2 = vx - delta;
            accel_to(target2);
            hold(rng.uniform_int(max(2, holdMin/3), holdMin));
        }
        // Hard clip to avoid runaway
        clip_vx(hardCap);

        // Rare full stop and restart to new angle
        if(rng.uniform() < 0.08){
            accel_to(0);
            hold(rng.uniform_int(holdMin/2, holdMin));
        }
    }

    // If any remainder due to bounds, fill with neutral C
    while(cnt < t){ emit('C'); ++cnt; }

    return 0;
}
