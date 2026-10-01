#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include <unordered_map>
#include <set>
#include <algorithm> // for find()
using namespace std;

// Generic function to print any vector
template <typename T>
void Print(const T& container){
    if constexpr (is_arithmetic_v<T>){
        cout << setw(3) << container;
    }
    else {
        for (auto& e : container){
            Print(e);
        }
        cout << "\n";
    }
}

// 29/9
int PartitionPainterR(vector<int>& arr, int k, int sz_remaining, vector<vector<int>>& dp){
    int cur_sum = 0;
    int n = arr.size();

    if (dp[k][sz_remaining] != -1) {
        printf("[%d][%d]\n", k, sz_remaining);
        return dp[k][sz_remaining];
    }
    // base case
    // for the remining size
    if (sz_remaining == 0) return dp[k][sz_remaining] = 0;
    if (sz_remaining == 1) return dp[k][sz_remaining] = arr[n - 1];

    if (k == 1){ // only 1 painter left
        for (int i = n - sz_remaining; i < n; i++)
            cur_sum += arr[i];

        return dp[k][sz_remaining] = cur_sum;
    }

    int maxx = INT_MIN;
    int min_in_max = INT_MAX;
    for (int i = sz_remaining - 1; i >= 1; i--){
        cur_sum += arr[n - i - 1];
        int rest = PartitionPainterR(arr, k - 1, i, dp);
        maxx = max(cur_sum, rest);

        // Choose min time among max time of painting
        min_in_max = min(min_in_max, maxx);
    }

    return dp[k][sz_remaining] = min_in_max;
}
int PartitionPainter(vector<int>& arr, int k){
    int n = arr.size();

    vector<vector<int>> dp(k + 1, vector<int>(n + 1, -1));

    int res = PartitionPainterR(arr, k, n, dp);

    printVectorVector(dp);

    return res;
}

// 30/9
int MinTimeReqR(vector<int> arr, int sz){
    if (sz == 1) return arr[0];
    if (sz == 2) return arr[1];
    if (sz == 3) return arr[0] + arr[1] + arr[2];
 
    // send the 2 fastest to remove the 2 slowest (sz - 2)
    int res1 = arr[1] + arr[0] + arr[sz - 1] + arr[1] + MinTimeReqR(arr, sz - 2);
 
    // Let the fastest to escort the slowest (sz - 1)
    int res2 = arr[sz - 1] + arr[0] + MinTimeReqR(arr, sz - 1);
   
    return min(res1, res2);
}
int MinTimeReq(vector<int> arr){
    int n = arr.size();
 
    return MinTimeReqR(arr, n);
}
 
int MinMultyMatrixR(vector<int> arr, int idx){
    if (idx >=0) arr.erase(arr.begin() + idx);
    int n = arr.size();
    if (n == 2) return 0;
 
    int res, minn = INT_MAX;
    for (int i = 1; i < n - 1; i++){
        res = arr[i - 1]*arr[i]*arr[i + 1] + MinMultyMatrixR(arr, i);
        minn = min(minn, res);
    }
 
    return minn;
}
int MinMultyMatrix(vector<int>& arr){
    int n = arr.size();
    if (n <= 2) return 0;
 
    return MinMultyMatrixR(arr, -1);
}
 
int MaxSumRec(vector<vector<int>> rec){
    int n = rec.size();
    int m = rec[0].size();
 
    vector<vector<vector<vector<int>>>> dp(
        n,
        vector<vector<vector<int>>>(
            m,
            vector<vector<int>>(
                n,
                vector<int>(m, -1)
            )
        )
    );
 
    int maxx = INT_MIN;
 
    for (int n_start = 0; n_start < n; n_start++){
        for (int m_start = 0; m_start < m; m_start++){
 
            // int cur_m_sum = 0;
            for (int cur_n = n_start; cur_n < n; cur_n++){
                int cur_m_sum = 0;
                for (int cur_m = m_start; cur_m < m; cur_m++){
 
                    if (cur_n - n_start >= 1) {

                        if (cur_m == m_start)
                            cur_m_sum += rec[cur_n][cur_m] + dp[n_start][m_start][cur_n - 1][cur_m];
                        else
                            cur_m_sum += rec[cur_n][cur_m] + dp[n_start][m_start][cur_n - 1][cur_m]
                                                        - dp[n_start][m_start][cur_n - 1][cur_m - 1];

                        // if (n_start == 1 and m_start == 1){
                        //     printf("[%d, %d]: %d\n", cur_n, cur_m, cur_m_sum);

                        //     printf("%d + %d - %d\n", rec[cur_n][cur_m], dp[n_start][m_start][cur_n - 1][cur_m],
                        //                                         dp[n_start][m_start][cur_n - 1][cur_m - 1]);
                        // }
                    }
                    else cur_m_sum += rec[cur_n][cur_m];

                    dp[n_start][m_start][cur_n][cur_m] = cur_m_sum;
 
                    maxx = max(maxx, cur_m_sum);
                }
            }
        }
    }
 
    //Print(dp[1][1]);
 
    return maxx;
}
 
int Kadane_array(vector<int>& arr){
    int n = arr.size();
    int cur_sum = 0;
    int maxx = INT_MIN;
    int Global_max = INT_MIN;

    for (int i = 0; i < n; i++){
        cur_sum += arr[i];

        maxx = cur_sum;
        if (arr[i] > cur_sum){
            maxx = arr[i];

            cur_sum = arr[i];
        }

        Global_max = max(maxx, Global_max);
    }

    return Global_max;
}
int MaxSumRec(vector<vector<int>> &mat){
    int rows = mat.size();
    int cols = mat[0].size();

    int maxx = INT_MIN;
    vector<int> temp_row(cols, -1);
    for (int up = 0; up < rows; up++){
        // reset temp_row
        for (int i = 0; i < cols; i++){
            temp_row[i] = 0;
        }

        for (int down = up; down < rows; down++){
            for (int i = 0; i < cols; i++){
                temp_row[i] += mat[down][i];
            }

            // Use kadane to find max
            maxx = max(maxx, Kadane_array(temp_row));
        }
    }

    return maxx;
}

// 1/10
int MaxProfit2TransR(vector<int>& prices, int idx, int n, vector<vector<int>>& dp){
    // base case
    if (n == 0) return 0;
    if (idx >= prices.size() - 1) return 0;
 
    if (dp[idx][n] != -1) {
        return dp[idx][n];
    }
 
    int res = 0, maxx = 0;
 
    for (int buy = idx; buy < prices.size() - 1; buy++){
        for (int sell = buy + 1; sell < prices.size(); sell++){
            if (prices[sell] > prices[buy]) {
                res = prices[sell] - prices[buy] + MaxProfit2TransR(prices, sell + 1, n - 1, dp);
                maxx = max(res, maxx);
            }
        }
    }
 
    return dp[idx][n] = maxx;
}
int MaxProfit2Trans(vector<int> prices){
    int n = prices.size();
    vector<vector<int>> dp(n, vector<int>(2 + 1, -1));
    return MaxProfit2TransR(prices, 0, 2, dp);
}
 
int NumOfAPR(vector<int>& arr, int idx, int sz, int difference){
    int n = arr.size();
 
    // printf("%d-%d-%d\n", idx, sz, difference);
 
    int res = 0;
    for (int i = idx + 1; i < n; i++){
        int diff = arr[i] - arr[idx];
 
        if (diff == difference and sz >= 3 or sz < 3){
            res += 1 + NumOfAPR(arr, i, sz + 1, diff);
        }
    }
 
    return res;
}
int NumOfAP(vector<int> arr){
    int n = arr.size();
 
    int num = 0;
    for (int i = 0; i < n - 1; i++){
        int sub = arr[i+1] - arr[i];
        num += NumOfAPR(arr, i,2, sub); // starting sz = 2
    }
 
    num += n + 1; // for {} only + single number
 
    return num;
}
 
bool IsAP(vector<int> arr){
    int n = arr.size();
    if (n <= 2) return true;
 
    int diff = arr[1] - arr[0];
    for (int i = 2; i < n; i++){
        if (arr[i] - arr[i - 1] != diff) return false;
    }
    return true;
}
int NumOfAPR1(vector<int> arr, int idx, vector<int> test){
 
    if (idx == arr.size()){
        bool ret = IsAP(test);
        return (ret == true) ? 1 : 0;
    }
 
    int res = 0;
 
    res = NumOfAPR1(arr, idx + 1, test);
 
    test.push_back(arr[idx]);
    res += NumOfAPR1(arr, idx + 1, test);
 
    return res;
}
int NumOfAP1(vector<int> arr){
    vector<int> test;
 
    // use this partition to generate all possible subseq, then check AP
    return NumOfAPR1(arr, 0, test);
}

int main(){
    int res;
    bool ret;
    float ress;
    double resss;

    cout << res << "\n";
}

