#include <bits/stdc++.h>
using namespace std;

/*
Heuristic controller without full physics.
Variant: multi-scale sweeps.
 - Perform sequences with increasing/decreasing target speeds to cover
   different angles. Between sweeps, insert resets (brief counter-kicks)
   to decorrelate trajectories.
*/

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k; if(!(cin>>k)) return 0;
    int n; cin>>n;
    long long m; cin>>m;
    int s; cin>>s;
    int N = 2*n;
    for(int i=0;i<N;i++) for(int j=0;j<N;j++){ int c; cin>>c; (void)c; }

    long long t = m;

    auto emit = [&](char ch){ cout<<ch<<'\n'; };

    long long cnt = 0;

    auto accel_to = [&](int &vx, int target){
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
            if(dir>0){ emit('D'); } else { emit('B'); }
            ++cnt;
        }
    };

    int baseV = max(6, n/3);
    vector<int> scales = {1, 2, 3, 1, 4, 2};
    int vx = 0;

    // Warm start
    accel_to(vx, min(baseV, 5)); hold(max(4, n*3));

    while(cnt < t){
        for(int sidx=0; sidx<(int)scales.size() && cnt<t; ++sidx){
            int V = baseV * scales[sidx];
            V = min(V, 5*baseV); // cap extreme velocities
            // Right sweep plateau
            accel_to(vx, V);
            hold(max(6, n*4));
            // Reset decorrelation: brief opposite nudges
            nudge(-1, max(1, V/6));
            // Left sweep plateau
            accel_to(vx, -V);
            hold(max(6, n*4));
            // Reset decorrelation
            nudge(+1, max(1, V/6));
        }
        // Mid-cycle neutral hold
        hold(max(8, n*2));
    }

    // Ensure exactly t lines printed (loop maintains it)
    return 0;
}
