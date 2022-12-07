#include <errno.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
extern int errno;
#define GAP 30;
// getrusage() is available in linux. Your code will be evaluated in Linux OS.
long getTotalMemory() {
    struct rusage usage;
    int returnCode = getrusage(RUSAGE_SELF, &usage);
    if (returnCode == 0) {
        return usage.ru_maxrss;
    } else {
        // It should never occur. Check man getrusage for more info to debug.
        //  printf("error %d", errno);
        return -1;
    }
}

int penalty_mismatch(char a, char b) {
    string DNA = "ACGT";
    int index1 = DNA.find(a);
    int index2 = DNA.find(b);
    int penalty_metrix[4][4] = {{0, 110, 48, 94},
                                {110, 0, 118, 48},
                                {48, 118, 0, 110},
                                {94, 48, 110, 0}};
    return penalty_metrix[index1][index2];
}

int min(int a, int b, int c) {
    if (a <= b && a <= c)
        return a;
    else if (b <= a && b <= c)
        return b;
    else if (c <= a && c <= b)
        return c;
    else
        return 0;
}

string* alignment(string seq1, string seq2) {
    string* pair = new string[2];
    pair[0] = "";
    pair[1] = "";
    int len1 = seq1.length();
    int len2 = seq2.length();
    int minValue = 0;
    int gap = GAP;
    int** opt = new int*[len1 + 1];
    for (int i = 0; i <= len1; i++) opt[i] = new int[len2 + 1];
    opt[0][0] = 0;
    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            if (i == 0) {
                opt[i][j] = j * GAP;
            } else if (j == 0) {
                opt[i][j] = i * GAP;
            } else {
                int top = opt[i - 1][j] + GAP;
                int left = opt[i][j - 1] + GAP;
                int diagonal = opt[i - 1][j - 1] + penalty_mismatch(seq1[i - 1], seq2[j - 1]);
                opt[i][j] = min(diagonal, top, left);
            }
        }
    }
    int i = len1;
    int j = len2;
    while (i != 0 && j != 0) {
        int left = opt[i][j - 1] + GAP;
        int top = opt[i - 1][j] + GAP;
        int diagonal = opt[i - 1][j - 1] + penalty_mismatch(seq1[i - 1], seq2[j - 1]);
        if (opt[i][j] == diagonal) {
            pair[0] += seq1[i - 1];
            pair[1] += seq2[j - 1];
            i--;
            j--;
        } else if (opt[i][j] == left) {
            pair[0] += "_";
            pair[1] += seq2[j - 1];
            j--;
        } else if (opt[i][j] == top) {
            pair[0] += seq1[i - 1];
            pair[1] += "_";
            i--;
        }
    }
    while (i > 0) {
        // append character of the string for which the index is changing to the corresponding stringbuilder
        // in this case, i is changing, hence we append sb1 with string a's character at i
        pair[0] += seq1[i - 1];
        pair[1] += "_";
        i--;
    }

    while (j > 0) {
        pair[0] += "_";
        // append character of the string for which the index is changing to the corresponding stringbuilder
        // in this case, j is changing, hence we append sb2 with string b's character at j
        pair[1] += seq2[j - 1];
        j--;
    }
    reverse(pair[0].begin(), pair[0].end());
    reverse(pair[1].begin(), pair[1].end());

    return pair;
}

void NWScore(string a, string b, vector<int>& p) {
    // creating 2 arrays - evenEdits and oddEdits that will store my currentArray and previousArray
    int* evenEdits = new int[b.length() + 1];
    int* oddEdits = new int[b.length() + 1];

    // Initializing the evenEdits array because we will always start from index 0 which is even
    for (int j = 0; j < b.length() + 1; j++) {
        evenEdits[j] = j * GAP;
    }

    // Initialize 2 arrays, currentEdits and previousEdits which will store our DP calculations
    int* currentEdits;
    int* previousEdits;

    // check which (from oddEdits and evenEdits) is the currentEdit and which one is the other
    // we traverse the while loop till big.length() because now we need to traverse the bigger string.
    // refer to the dp[][] array in Solution1 to draw parallels
    for (int i = 1; i < a.length() + 1; i++) {
        if (i % 2 == 0) {
            currentEdits = evenEdits;
            previousEdits = oddEdits;
        } else {
            currentEdits = oddEdits;
            previousEdits = evenEdits;
        }

        // this is same as initializing the dp[][] array's first column
        currentEdits[0] = i * GAP;

        for (int j = 1; j < b.length() + 1; j++) {
            int left = currentEdits[j - 1] + GAP;
            int top = previousEdits[j] + GAP;
            int diagonal = previousEdits[j - 1] + penalty_mismatch(a[i - 1], b[j - 1]);
            currentEdits[j] = min(diagonal, left, top);
        }
    }
    // depending on the length of the bigger string (which spans across the rows in the dp[][] arrray)
    // the answer might be stored in either the evenEdits[] or the oddEdits[] array
    if (a.length() % 2 == 0) {
        for (int i = 0; i <= b.length(); i++) {
            p[i] = evenEdits[i];
        }
    } else {
        for (int i = 0; i <= b.length(); i++) {
            p[i] = oddEdits[i];
        }
    }
}

void dc_alignment(string seq1, string seq2, string* result) {
    string* temp = new string[2];
    temp[0] = "";
    temp[1] = "";
    if (seq1.length() == 0) {
        for (int i = 0; i < seq2.length(); i++) {
            result[0] += "_";
            result[1] += seq2[i];
        }
        return;
    } else if (seq2.length() == 0) {
        for (int i = 0; i < seq1.length(); i++) {
            result[0] += seq1[i];
            result[1] += "_";
        }
        return;
    } else if (seq1.length() == 1 || seq2.length() == 1) {
        temp = alignment(seq1, seq2);
        result[0] += temp[0];
        result[1] += temp[1];
        return;
    } else {
        int len1 = seq1.length();
        int mid1 = len1 / 2;
        int len2 = seq2.length();
        vector<int> L(len2 + 1, 0);
        vector<int> R(len2 + 1, 0);
        NWScore(seq1.substr(0, mid1), seq2, L);
        string arev = seq1.substr(mid1);
        reverse(arev.begin(), arev.end());
        string brev = seq2;
        reverse(brev.begin(), brev.end());
        NWScore(arev, brev, R);
        reverse(R.begin(), R.end());
        int min = 999999;
        int mid2 = 0;
        for (int i = 0; i <= len2; i++) {
            int sum = L[i] + R[i];
            if (sum < min) {
                min = sum;
                mid2 = i;
            }
        }
        dc_alignment(seq1.substr(0, mid1), seq2.substr(0, mid2), result);
        dc_alignment(seq1.substr(mid1), seq2.substr(mid2), result);
    }
}

int Cost(string* pair) {
    string seq1 = pair[0];
    string seq2 = pair[1];
    int cost = 0;
    for (int i = 0; i < seq1.length(); i++) {
        char c1 = seq1[i];
        char c2 = seq2[i];
        if (c1 == '_' || c2 == '_') {
            cost += GAP;
        } else {
            cost += penalty_mismatch(seq1[i], seq2[i]);
        }
    }
    return cost;
}

int main(int argc, char* argv[]) {
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    // Read File from the input.txt;
    string base1, base2;
    string line;
    ifstream myfile(argv[1]);

    // Read 2 sequence from file
    myfile >> base1;
    string seq1 = "";
    string seq2 = "";
    seq1 = base1;
    int index = 0;
    myfile >> line;
    while (line.at(0) != 'A' && line.at(0) != 'T' && line.at(0) != 'C' && line.at(0) != 'G') {
        stringstream buf(line);
        buf >> index;
        seq1.insert(index + 1, base1);
        base1 = seq1;
        myfile >> line;
    }
    base2 = line;
    seq2 = base2;

    while (!myfile.eof()) {
        myfile >> index;
        seq2.insert(index + 1, base2);
        base2 = seq2;
    }
    string* pair = new string[2];
    pair[0] = "";
    pair[1] = "";
    dc_alignment(seq1, seq2, pair);
    // cout << "seq1: " << pair[0] << endl;
    // cout << "seq2: " << pair[1] << endl;
    double totalmemory = getTotalMemory();
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double totaltime = seconds * 1000 + microseconds * 1e-3;
    /* cout << "The total time cost: " << totaltime << "sec" << endl;
     cout << "The total memory cost: " << totalmemory / 1024 << "KiB" << endl;
     cout << "The total cost: " << Cost(pair) << endl;*/

    // Ouput the result
    ofstream outputfile(argv[2]);
    outputfile << Cost(pair) << endl;
    outputfile << pair[0] << endl;
    outputfile << pair[1] << endl;
    outputfile << totaltime << "sec" << endl;
    outputfile << totalmemory / 1024 << "KiB" << endl;

    // Using Bash file to generate the plot for the Execution Time and MemoryUsage for the input point
    // Please uncomment the following code then running sudo sh generate_point.sh
    /* ofstream PlotEfficient(argv[2], fstream::app);
    int totalSize = seq1.size() + seq2.size();
    PlotEfficient << totalSize << "," << totaltime << "," << totalmemory / 1024 << endl; */
}
