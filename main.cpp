#include <bits/stdc++.h>
using namespace std;

/*
Heuristic controller for Snooker Brick Out without full physics.
We output a structured sequence of operations that "servo" the
horizontal velocity to sweep right and left with long constant
segments, hoping to cover the board better than a short repeating
pattern.

Controls:
 A: vx -= 2 (left)
 B: vx -= 1 (left)
 C: vx += 0
 D: vx += 1 (right)
 E: vx += 2 (right)

Strategy:
 - Maintain an internal vx variable (our intended horizontal speed).
 - Repeatedly perform two sweeps:
   1) Accelerate to +V using E/D, then hold with P times C
   2) Accelerate to -V using A/B, then hold with P times C
 - Repeat until we emit t operations (choose t = m).
 - Choose parameters V and P as functions of n to adapt scale.

Notes:
 - This ignores colors; without simulator we cannot target 1-2-3 chain.
 - Using long C segments reduces unnecessary oscillations.
*/

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k; if(!(cin>>k)) return 0;
    int n; cin>>n;
    long long m; cin>>m;
    int s; cin>>s;
    int N = 2*n;
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            int c; cin>>c; (void)c;
        }
    }

    long long t = m; // number of operations to output

    // Parameter selection
    // V: target horizontal velocity magnitude
    // P: number of holds (C) at each plateau
    int V = max(6, n/3);           // grows mildly with n
    int P = max(8, n*6);           // long enough to travel across

    long long emitted = 0;
    int vx = 0; // intended vx we keep in sync with operations

    auto push_right_to = [&](int target){
        while(emitted < t && vx < target){
            int need = target - vx;
            if(need >= 2){ cout << 'E' << '\n'; vx += 2; }
            else { cout << 'D' << '\n'; vx += 1; }
            ++emitted;
        }
    };
    auto push_left_to = [&](int target){
        while(emitted < t && vx > target){
            int need = vx - target;
            if(need >= 2){ cout << 'A' << '\n'; vx -= 2; }
            else { cout << 'B' << '\n'; vx -= 1; }
            ++emitted;
        }
    };
    auto hold = [&](int cnt){
        for(int i=0;i<cnt && emitted < t;i++){
            cout << 'C' << '\n';
            ++emitted;
        }
    };

    // Warm-up: small right bias to break symmetry
    push_right_to(min(V, 4));
    hold(P/2);

    while(emitted < t){
        // Right sweep
        push_right_to(V);
        hold(P);
        if(emitted >= t) break;
        // Left sweep
        push_left_to(-V);
        hold(P);
    }

    // If underflow due to rounding, fill remaining with neutral C
    while(emitted < t){ cout << 'C' << '\n'; ++emitted; }

    return 0;
}
