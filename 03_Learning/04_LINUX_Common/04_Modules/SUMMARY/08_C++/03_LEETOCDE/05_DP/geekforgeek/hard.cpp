#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include <unordered_map>
#include <set>
#include <map>
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

// 2/10
int LongestSubarrayR(string& s1, int i, string& s2, int j, vector<vector<int>>& dp){
    if (i < 0 or j < 0) return 0;
 
    int commonLen = 0;
    int maxx = 0;
 
    if (dp[i][j] != -1) {
        //printf("%d, %d\n", i, j);
        return dp[i][j];
    }
 
    int tmp_i = i, tmp_j = j;
    while ((tmp_i >= 0 and tmp_j >= 0) and s1[tmp_i] == s2[tmp_j]){
        commonLen++;
        tmp_i--;
        tmp_j--;
    }
 
    int len1, len2;
    len1 = LongestSubarrayR(s1, i - 1, s2, j, dp);
    len2 = LongestSubarrayR(s1, i, s2, j - 1, dp);
 
    maxx = max(commonLen, max(len1, len2));
 
    return dp[i][j] = maxx;
}
int LongestSubarray(string s1, string s2){
    int n1 = s1.length();
    int n2 = s2.length();
 
    vector<vector<int>> dp(n1, vector<int>(n2, -1));
 
    return LongestSubarrayR(s1, n1 - 1, s2, n2 - 1, dp);
}
 
int LongestCommonStrR(string& s, int i, int j, vector<vector<int>>& dp){
    int n = s.length();
    if (i >= j) return 0;
   
    if (i == n - 1 or j == n) return 0;
 
    if (dp[i][j] != -1){
        return dp[i][j];
    }
 
    int res = 0;
    int x = i, y = j;
    while ((x < j and y < n) and s[x] == s[y]){
        res++;
        x++;
        y++;
    }
 
    int res1, res2;
    res1 = LongestCommonStrR(s, i + 1, j, dp);
    res2 = LongestCommonStrR(s, i, j + 1, dp);
 
    return dp[i][j] = max(res, max(res1, res2));
}
string LongestCommonStr(string& s){
    int n = s.length();
 
    string out;
    vector<vector<int>> dp(n, vector<int>(n, -1));
    int res;
    int maxx = 0;
    int i_max = 0;
 
    res = LongestCommonStrR(s, 0, 1, dp);
 
    for (int i = n - 1; i >= 0; i--){
        for (int j = n - 1; j > i; j--){
            if (maxx < dp[i][j]){
                maxx = dp[i][j];
                i_max = i;
            }
        }
    }
 
    for (int i = i_max; i < i_max + maxx; i++){
        out.push_back(s[i]);
    }
 
    return out;
}

int PartitionArrayR(vector<int> arr, int k, int idx, vector<vector<int>>& dp){
    int n = arr.size();
    if (idx == n) return 0;

    if (dp[idx][k] != -1) return dp[idx][k];

    int res;
    int cur_num_max;
    int maxx = INT_MIN;
    for (int i = idx; i < min(idx + k, n); i++){
        cur_num_max = INT_MIN;
        for (int j = idx; j <= i; j++){
            cur_num_max = max(arr[j], cur_num_max);
        }

        res = (i - idx + 1)*cur_num_max + PartitionArrayR(arr, k, i + 1, dp);
        maxx = max(maxx, res);
    }

    dp[idx][k] = maxx;

    return maxx;
}
int PartitionArray(vector<int> arr, int k){
    int n = arr.size();
    vector<vector<int>> dp(n, vector<int>(k + 1, -1));
    return PartitionArrayR(arr, k, 0, dp);
}

// 3/10
int ShortCommonR(string s1, string s2, int idx1, int idx2){
    // base case
    if (idx1 < 0 or idx2 < 0) return 0;

    int res1 = 0, res2 = 0, res3 = 0;
    int maxx = 0;
    if (s1[idx1] == s2[idx2]){
        res1 = 1 + ShortCommonR(s1, s2, idx1 - 1, idx2 - 1);
    }
    else {
        res2 = ShortCommonR(s1, s2, idx1 - 1, idx2);

        res3 = ShortCommonR(s1, s2, idx1, idx2 - 1);
    }

    maxx = max(max(res2, res3), res1);

    return maxx;
}
int ShortCommon(string s1, string s2){
    int n1 = s1.length();
    int n2 = s2.length();

    int res = ShortCommonR(s1, s2, n1 - 1, n2 - 1);

    return res + (n1 - res) + (n2 - res);
}

int ShortCommonR1(string s1, string s2, int idx1, int idx2){
    // base case
    if (idx1 < 0){
        return idx2 + 1;
    }
    if (idx2 < 0){
        return idx1 + 1;
    }

    int res1 = 0, res2 = 0, res3 = 0;
    int minn = 0;
    if (s1[idx1] == s2[idx2]){
        res1 = 1 + ShortCommonR1(s1, s2, idx1 - 1, idx2 - 1);

        minn = res1;
    }
    else {
        res2 = 1 + ShortCommonR1(s1, s2, idx1 - 1, idx2);
        res3 = 1 + ShortCommonR1(s1, s2, idx1, idx2 - 1);

        minn = min(res2, res3);
    }

    return minn;
}
int ShortCommon1(string s1, string s2){
    int n1 = s1.length();
    int n2 = s2.length();

    return ShortCommonR1(s1, s2, n1 - 1, n2 - 1);
}

int longestRepeatedSubSeq1R(string& s, int idx1, int idx2, vector<vector<int>>& dp){
    // base case
    int n = s.length();
    if (idx1 >= n or idx2 >= n) return 0;

    if (dp[idx1][idx2] != -1) {
        printf("%d, %d\n", idx1, idx2);
        return dp[idx1][idx2];
    }

    int take = 0, not_take = 0;
    if (idx1 != idx2 and s[idx1] == s[idx2]){
        take = 1 + longestRepeatedSubSeq1R(s, idx1 + 1, idx2 + 1, dp);
    }    

    int res1, res2;
    res1 = longestRepeatedSubSeq1R(s, idx1 + 1, idx2, dp);
    res2 = longestRepeatedSubSeq1R(s, idx1, idx2 + 1, dp);

    not_take = max(res1, res2);

    return dp[idx1][idx2] = max(take, not_take);
}
int longestRepeatedSubSeq1(string s){
    int n = s.length();
    vector<vector<int>> dp(n, vector<int>(n, -1));

    return longestRepeatedSubSeq1R(s, 0, 0, dp);
}

string longestRepeatedSubSeqStrR(string& s, int idx1, int idx2, vector<vector<string>>& dp, string add_str){
    // base case
    int n = s.length();
    if (idx1 >= n or idx2 >= n) return add_str;

    if (dp[idx1][idx2] != "") {
        //printf("%d, %d\n", idx1, idx2);
        return dp[idx1][idx2];
    }

    string out, out1, out2;
    string maxx;
    if (idx1 != idx2 and s[idx1] == s[idx2]){
        add_str.push_back(s[idx1]);
        out = longestRepeatedSubSeqStrR(s, idx1 + 1, idx2 + 1, dp, add_str);
        add_str.pop_back();
    }

    out1 = longestRepeatedSubSeqStrR(s, idx1 + 1, idx2, dp, add_str);
    out2 = longestRepeatedSubSeqStrR(s, idx1, idx2 + 1, dp, add_str);

    if (out1.length() > out2.length()){
        maxx = out1;
    }
    else {
        maxx = out2;
    }

    if (maxx.size() < out.size()){
        maxx = out;
    }

    return dp[idx1][idx2] = maxx;
}
string longestRepeatedSubSeqStr(string& s){
    int n = s.length();
    vector<vector<string>> dp(n, vector<string>(n, ""));

    return longestRepeatedSubSeqStrR(s, 0, 0, dp, "");
}

/*
Alternative ways: 
 + Find the max len of lcs
 + Delete = n_s1 - len
 + Insert = n_s2 - len
 => total = Delete + Insert
*/
int MinDeleteOrInsertR(string s1, int i1, string s2, int i2){

    // base case
    if (i1 < 0) {
        return i2 + 1; // for inserting the rest of s2 on s1
    }
    if (i2 < 0){
        return i1 + 1; // for delete the rest of s2 on s1
    }

    int res1 = INT_MAX, res2 = INT_MAX;
    int minn = INT_MAX;
    if (s1[i1] == s2[i2]){
        res1 = MinDeleteOrInsertR(s1, i1 - 1, s2, i2 - 1);
    }

    else {
        // delete on s1
        res1 = 1 + MinDeleteOrInsertR(s1, i1 - 1, s2, i2);

        // insert on s1
        res2 = 1 + MinDeleteOrInsertR(s1, i1, s2, i2 - 1);
    }

    return minn = min(res1, res2);
}
int MinDeleteOrInsert(string s1, string s2){
    int n1 = s1.length();
    int n2 = s2.length();

    return MinDeleteOrInsertR(s1, n1 - 1, s2, n2 - 1);
}

// 4/10
const long long MOD = 1000000007;
int CountDistinctSubseq(const string& str)
{
    int n = str.length();

    vector<long long> dp(n + 1, 0);

    // dp[0] = empty subsequence
    dp[0] = 1;

    // last[c] = 1-based position where c was previously seen
    unordered_map<char, int> last;

    for (int i = 1; i <= n; i++)
    {
        char c = str[i - 1];

        // Take or don't take current character
        dp[i] = (2 * dp[i - 1]) % MOD;

        if (last.count(c))
        {
            int prev = last[c];

            // Remove duplicates created by the previous occurrence
            dp[i] = (dp[i] - dp[prev - 1] + MOD) % MOD;
        }

        last[c] = i;
    }

    return dp[n];
}

int LCS3R(string s1, int i1, string s2, int i2, string s3, int i3,
                                    vector<vector<vector<int>>>& dp){
    if (i1 < 0 or i2 < 0 or i3 < 0) return 0;

    if (dp[i1][i2][i3] != -1) {
        return dp[i1][i2][i3];
    }

    int res1 = 0, res2, res3, res4;

    if (s1[i1] == s2[i2] and s2[i2] == s3[i3]){
        res1 = 1 + LCS3R(s1, i1 - 1, s2, i2 - 1, s3, i3 - 1, dp);
    }

    res2 = LCS3R(s1, i1 - 1, s2, i2, s3, i3, dp);
    res3 = LCS3R(s1, i1, s2, i2 - 1, s3, i3, dp);
    res4 = LCS3R(s1, i1, s2, i2, s3, i3 - 1, dp);

    return dp[i1][i2][i3] = max(max(res1, res2), max(res3, res4));
}
int LCS3(string s1, string s2, string s3){
    int n1 = s1.length();
    int n2 = s2.length();
    int n3 = s3.length();

    vector<vector<vector<int>>> dp(
        n1,
        vector<vector<int>>(
            n2, 
            vector<int>(
                n3, -1
            )
        )
    );

    return LCS3R(s1, n1 - 1, s2, n2 - 1, s3, n3 - 1, dp);
}

void CntOfLISR(vector<int> vec, int idx, int prev, map<int, int>& Idx2Val, int sz){
    // base case
    int n = vec.size();
    if (idx == n){
        Idx2Val[sz]++;
        return;
    }
    int res1 = 0, res2;
    if (prev == -1 or vec[idx] > vec[prev]){
        CntOfLISR(vec, idx + 1, idx, Idx2Val, sz + 1);
    }

    CntOfLISR(vec, idx + 1, prev, Idx2Val, sz);
}
int CntOfLIS(vector<int> vec){
    int n = vec.size();
    map<int, int> Idx2Val;
    int sz = 0;

    CntOfLISR(vec, 0, -1, Idx2Val, sz);

    int maxKey;
    if (!Idx2Val.empty()){
        maxKey = Idx2Val.begin()->second; // or .rbegin()->first;
        return Idx2Val[maxKey];
    }
    return 0;
}

bool isAP(vector<int> v){
    int n = v.size();

    if (n <= 2) return true;

    int diff = v[1] - v[0];
    for (int i = 2; i < n; i++){
        if (v[i] - v[i - 1] != diff) return false;
    }
    return true;
}
void LongestArithmeticR(vector<int> vec, int idx, vector<vector<int>>& vecStr, vector<int> tmp){
    int n = vec.size();
    if (idx >= n){
        if (isAP(tmp)) {
            vecStr.push_back(tmp);
        }
        return;
    }

    // not take
    LongestArithmeticR(vec, idx + 1, vecStr, tmp);

    // take
    tmp.push_back(vec[idx]);
    LongestArithmeticR(vec, idx + 1, vecStr, tmp);
}
int LongestArithmetic(vector<int> vec){

    vector<vector<int>> vecStr;
    vector<int> tmp;
    LongestArithmeticR(vec, 0, vecStr, tmp);

    int maxx = 0;
    for (auto& a : vecStr){
        int sz = a.size();
        maxx = max(sz, maxx);
    }

    return maxx;
}

void LongestArithmetic1R(vector<int> vec, int idx, int prev, int diff, vector<vector<int>>& vecStr, vector<int> tmp, int sz_taken){
    int n = vec.size();
    if (idx == n){
        vecStr.push_back(tmp);
        return;
    }

    // don't take
    LongestArithmetic1R(vec, idx + 1, prev, diff, vecStr, tmp, sz_taken);

    // take
    if (sz_taken <= 1 or vec[idx] - vec[prev] == diff){
        tmp.push_back(vec[idx]);

        // case only when one number is existed
        if (sz_taken == 0) LongestArithmetic1R(vec, idx + 1, idx, -1, vecStr, tmp, sz_taken + 1);
        // the first time to take diff
        if (sz_taken == 1) LongestArithmetic1R(vec, idx + 1, idx, vec[idx] - vec[prev], vecStr, tmp, sz_taken + 1);
        // diff is the same
        else LongestArithmetic1R(vec, idx + 1, idx, diff, vecStr, tmp, sz_taken + 1);
    }
}
int LongestArithmetic1(vector<int> vec){
    vector<vector<int>> vecStr;
    vector<int> tmp;
    LongestArithmetic1R(vec, 0, -1, -1, vecStr, tmp, 0);

    //Print(vecStr);
    int maxx = 0;
    for (auto& a : vecStr){
        int sz = a.size();
        maxx = max(sz, maxx);
    }

    return maxx;
}

/*
    lis_table and lds_table here start from the current idx
*/
int LongestBitonic1(vector<int> vec) {
    int n = vec.size();

    // ending idx of lis
    vector<int> lis_table(n ,1);
    for (int i = 1; i < n; i++){
        for (int j = 0; j < i; j++){
            if (vec[i] > vec[j]){
                lis_table[i] = max(lis_table[i], lis_table[j] + 1);
            }
        }
    }

    // starting idx of lds
    vector<int> lds_table(n ,1);
    for (int i = n - 1; i >= 0; i--){
        for (int j = n - 1; j > i; j--){
            if (vec[i] > vec[j]){
                lds_table[i] = max(lds_table[i], lds_table[j] + 1);
            }
        }
    }

    Print(lis_table);
    Print(lds_table);

    int maxx = 0;
    for (int i = 1; i < n - 1; i++){
        maxx = max(maxx, lis_table[i] + lds_table[i] - 1);
    }

    return maxx;
}

// 5/10
int AlternatingSubseqR(vector<int> vec, int idx, bool inc, vector<vector<int>>& dp){
    int n = vec.size();
    if (idx == n - 1) {
        return 1;
    }
 
    if (dp[idx][inc] != -1){
        printf("->%d\n", idx);
        return dp[idx][inc];
    }
 
    int res = 0;
    int maxx = 0;
    if (inc == true){
        for (int i = idx + 1; i < n; i++){
            if (vec[idx] < vec[i]){
                res = 1 + AlternatingSubseqR(vec, i, !inc, dp);
                maxx = max(maxx, res);
            }
        }
    }
    else {
        for (int i = idx + 1; i < n; i++){
            if (vec[idx] > vec[i]){
                res = 1 + AlternatingSubseqR(vec, i, !inc, dp);
                maxx = max(maxx, res);
            }
        }      
    }
 
    return dp[idx][inc] = maxx;
}
int AlternatingSubseq(vector<int> vec){
    int n = vec.size();
 
    if (n <= 2) return 0;
 
    vector<vector<int>> dp(n, vector<int>(2, -1));
 
    int upfirst, downfirst;
    int maxx = 0;
    for (int i = 0; i < n; i++){
        upfirst = AlternatingSubseqR(vec, i, true, dp);
        downfirst = AlternatingSubseqR(vec, i, false, dp);
 
        //printf("%d %d\n", upfirst, downfirst);
        maxx = max(maxx, max(upfirst, downfirst));
    }
 
    if (maxx <= 2) return 0;
 
    return maxx;
}
 
int AlternatingSubseq1R(vector<int> vec, int idx, int prev, bool inc, vector<vector<vector<int>>>& dp){
    int n = vec.size();
    if (idx == n){
        return 0;
    }
 
    if (dp[idx][prev+1][inc] != -1){
        printf("##\n");
        return dp[idx][prev+1][inc];
    }
 
    int maxx = 0;
    // always take when when prev = -1
    if (prev == -1){
        int in = 0;
        in = 1 + AlternatingSubseq1R(vec, idx + 1, idx, inc, dp);
 
        maxx = in;
    }
    else {
        int in = 0, ex;
        if (inc == true){
            if (vec[prev] < vec[idx]){
                in = 1 + AlternatingSubseq1R(vec, idx + 1, idx, !inc, dp);
            }
            ex = AlternatingSubseq1R(vec, idx + 1, prev, inc, dp);
        }
        else {
            if (vec[prev] > vec[idx]){
                in = 1 + AlternatingSubseq1R(vec, idx + 1, idx, !inc, dp);
            }
            ex = AlternatingSubseq1R(vec, idx + 1, prev, inc, dp);
        }
 
        maxx = max(in, ex);
    }
 
    return dp[idx][prev+1][inc] = maxx;
}
int AlternatingSubseq1(vector<int> vec){
    int n = vec.size();
 
    if (n <= 2) return 0;
 
    vector<vector<vector<int>>> dp(
        n, vector<vector<int>>(
            n + 1, vector<int>(
                2, -1
            )
        )
    );
    int upfirst = AlternatingSubseq1R(vec, 0, -1, true, dp);
    int downfirst = AlternatingSubseq1R(vec, 0, -1, false, dp);
 
    int res = max(upfirst, downfirst);
 
    if (res <= 2) return 0;
    return res;
}

// 6/10
int ParenthesizationR(string s, int start_idx, int sz, bool ret, vector<vector<vector<int>>>& dp){
    // base case
    if (sz == 1){
        if (ret == true) {
            if (s[start_idx] == 'T') return 1;
            else return 0;
        }
        else {
            if (s[start_idx] == 'F') return 1;
            else return 0;  
        }
    }
 
    if (dp[start_idx][sz][ret] != -1){
        printf("##\n");
        return dp[start_idx][sz][ret];
    }
 
    int res = 0;
    int res_l_T = 0, res_r_T = 0;
    int res_l_F = 0, res_r_F = 0;
    for (int i = start_idx; i < start_idx + sz - 2; i += 2){
        res_l_T = ParenthesizationR(s, start_idx, i - start_idx + 1, true, dp);
        res_r_T = ParenthesizationR(s, i+2, start_idx + sz - (i + 2), true, dp);
 
        res_l_F = ParenthesizationR(s, start_idx, i - start_idx + 1, false, dp);
        res_r_F = ParenthesizationR(s, i+2, start_idx + sz - (i + 2), false, dp);
 
        if (ret == true){
            if (s[i+1] == '&'){
                res += res_l_T*res_r_T;
            }
            else if (s[i+1] == '|'){
                res += res_l_T*res_r_T;
                res += res_l_T*res_r_F;
                res += res_l_F*res_r_T;
            }
            else if (s[i+1] == '^'){
                res += res_l_T*res_r_F;
                res += res_l_F*res_r_T;
            }
        }
        else {
            if (s[i+1] == '&'){
                res += res_l_T*res_r_F;
                res += res_l_F*res_r_T;
                res += res_l_F*res_r_F;
            }
            else if (s[i+1] == '|'){
                res += res_l_F*res_r_F;
            }
            else if (s[i+1] == '^'){
                res += res_l_T*res_r_T;
                res += res_l_F*res_r_F;
            }          
        }
    }
 
    return dp[start_idx][sz][ret] = res;
}
int Parenthesization(string s){
    int n = s.size();
 
    int res = 0;
    int res_l_T = 0, res_r_T = 0;
    int res_l_F = 0, res_r_F = 0;
    bool ret;
 
    if (n == 1){
        if (s[0] == 'T') return 1;
        else return 0;
    }
 
    vector<vector<vector<int>>> dp(
        n, vector<vector<int>>(
            n + 1, vector<int>(
                2, -1
            )
        ));
 
    return ParenthesizationR(s, 0, n, true, dp);
}
 
bool CheckPalindrome(string s, int i, int j){
    if (i == j) return true;
    while(i <= j and s[i] == s[j]){
        i++;
        j--;
    }
 
    if (i <= j) return false;
    else return true;
}
int MinCutPalindromeR(string& s, int idx, int sz, vector<vector<int>>& dp){
    int n = s.length();
    //printf("%d, %d\n", idx, sz);
 
    if (sz <= 1) return 0;
 
    if (dp[idx][sz] != -1) {
        printf("%d, %d\n", idx, sz);
        return dp[idx][sz];
    }
 
    int end = idx + sz - 1;
    if (CheckPalindrome(s, idx, end)) {
        return dp[idx][sz] = 0;
    }
 
    int res_l = 0, res_r = 0;
    int res = 0;
    int minn = INT_MAX;
    for (int i = idx + 1; i < idx + sz; i++){
 
        int leftSize, rightSize;
        // sz
        leftSize = i - idx;
        rightSize = end - i + 1;
 
        res_l = MinCutPalindromeR(s, idx, leftSize, dp);
        res_r = MinCutPalindromeR(s, i, rightSize, dp);
 
        res = 1 + res_l + res_r;
       
        // if (idx == 0 and sz == 3) {
        //     printf("idx[%d,%d] -> sz[%d,%d] -> %d\n", idx, idx + leftSize - 1, idx, leftSize, res_l);
        //     printf("idx[%d,%d] -> sz[%d,%d] -> %d\n", i, i + rightSize - 1, i, rightSize, res_r);
        //     printf("res[%d] = %d\n=====\n", idx, res);
        // }
 
        // if (idx == 1 and sz == 3) {
        //     printf("[%d,%d] -> [%d,%d] -> %d\n", idx, idx + leftSize - 1, idx, leftSize, res_l);
        //     printf("[%d,%d] -> [%d,%d] -> %d\n", i, i + rightSize - 1, i, rightSize, res_r);
        //     printf("res[%d] = %d\n=====\n", idx, res);
        // }
 
        minn = min(res, minn);
    }
 
    return dp[idx][sz] = minn;
}
int MinCutPalindrome(string& s){
    int n = s.length();
    if (CheckPalindrome(s,0, n - 1)) return 0;
 
    vector<vector<int>> dp(n, vector<int>(n + 1, -1));
    return MinCutPalindromeR(s, 0, n, dp);
}

int DoubleKnapsackR(vector<int>& vec, int idx, int cap1, int cap2){
    if (idx < 0){
        return 0;
    }

    int not_take;
    int take1 = 0, take2 = 0;
    // dont take
    not_take = DoubleKnapsackR(vec, idx - 1, cap1, cap2);

    // take
    if (cap1 >= vec[idx]) take1 = vec[idx] + DoubleKnapsackR(vec, idx - 1, cap1 - vec[idx], cap2);
    if (cap2 >= vec[idx]) take2 = vec[idx] + DoubleKnapsackR(vec, idx - 1, cap1, cap2 - vec[idx]);

    if (idx == 0) {
        printf("%d, %d, %d\n", not_take, take1, take2);
    }

    return max(not_take, max(take1, take2));
}
int DoubleKnapsack(vector<int>& vec, int cap1, int cap2){
    int n = vec.size();

    return DoubleKnapsackR(vec, n - 1, cap1, cap2);
}

// 7/10
int Permutation_KInversion(int n, int k){
    if (k == 0) return 1;
    if (k < 0) return 0;
 
    int res = 0;
    for (int i = 0; i < n; i++){
        int tmp;
        tmp = Permutation_KInversion(n - 1, k - i);
        res += tmp;
 
        // if (n == 3){
        //     printf("[%d] = %d\n", i, tmp);
        // }
 
        /*
            i = 0, the smallest num is permutating itself -> k = k - 0 = k -> k is the same, then recur with the n - 1
            i = 1, the smallest num is permutating with the next -> k = k - 1 -> k is minus 1, then recur with the n - 1
            i = 2, the smallest num is permutating with the next next -> k = k - 2 -> k is minus 2, then recur with the n - 1
        */
    }
 
    return res;
}

int NumOfStairs123(int n){
    // if (n <= 1) return n;
    // if (n == 2) return n;
    // if (n == 3) return 4;
    if (n == 0) return 1;
    if (n < 0) return 0;

    int res1 = NumOfStairs123(n - 1);
    int res2 = NumOfStairs123(n - 2);
    int res3 = NumOfStairs123(n - 3);

    return res1 + res2 + res3;
}

int main(){
    int res;
    bool ret;
    float ress;
    double resss;

    cout << res << "\n";
}

