#include <errno.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;
extern int errno;

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

void printAlignment(string& seq1, string& seq2, string& sb1, string& sb2, int** opt, int len1, int len2) {
    int i = len1;
    int j = len2;
    int gap = 30;
    while (i != 0 && j != 0) {
        int left = opt[i][j - 1] + gap;
        int top = opt[i - 1][j] + gap;
        int diagonal = opt[i - 1][j - 1] + penalty_mismatch(seq1[i - 1], seq2[j - 1]);
        if (opt[i][j] == diagonal) {
            sb1 += seq1[i - 1];
            sb2 += seq2[j - 1];
            i--;
            j--;
        } else if (opt[i][j] == left) {
            sb1 += "_";
            sb2 += seq2[j - 1];
            j--;
        } else if (opt[i][j] == top) {
            sb1 += seq1[i - 1];
            sb2 += "_";
            i--;
        }
    }

    while (i > 0) {
        // append character of the string for which the index is changing to the corresponding stringbuilder
        // in this case, i is changing, hence we append sb1 with string a's character at i
        sb1 += seq1[i - 1];
        sb2 += "_";
        i--;
    }

    while (j > 0) {
        sb1 += "_";
        // append character of the string for which the index is changing to the corresponding stringbuilder
        // in this case, j is changing, hence we append sb2 with string b's character at j
        sb2 += seq2[j - 1];
        j--;
    }
    reverse(sb1.begin(), sb1.end());
    reverse(sb2.begin(), sb2.end());
    cout << "align1: " << sb1 << endl;
    cout << "align2: " << sb2 << endl;
}

void alignment(string seq1, string seq2, int** opt, string& sb1, string& sb2) {
    int gap = 30;
    int len1 = seq1.length();
    int len2 = seq2.length();
    int minValue = 0;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (seq1[i - 1] == seq2[j - 1]) {
                minValue = min(opt[i - 1][j - 1], opt[i - 1][j] + gap, opt[i][j - 1] + gap);
                opt[i][j] = minValue;
            } else {
                minValue = min(opt[i - 1][j - 1] + penalty_mismatch(seq1[i - 1], seq2[j - 1]), opt[i - 1][j] + gap, opt[i][j - 1] + gap);
                opt[i][j] = minValue;
            }
        }
    }
    // return opt[len1][len2];
    int i = len1;
    int j = len2;
    while (i != 0 && j != 0) {
        int left = opt[i][j - 1] + gap;
        int top = opt[i - 1][j] + gap;
        int diagonal = opt[i - 1][j - 1] + penalty_mismatch(seq1[i - 1], seq2[j - 1]);
        if (opt[i][j] == diagonal) {
            sb1 += seq1[i - 1];
            sb2 += seq2[j - 1];
            i--;
            j--;
        } else if (opt[i][j] == left) {
            sb1 += "_";
            sb2 += seq2[j - 1];
            j--;
        } else if (opt[i][j] == top) {
            sb1 += seq1[i - 1];
            sb2 += "_";
            i--;
        }
    }

    while (i > 0) {
        // append character of the string for which the index is changing to the corresponding stringbuilder
        // in this case, i is changing, hence we append sb1 with string a's character at i
        sb1 += seq1[i - 1];
        sb2 += "_";
        i--;
    }

    while (j > 0) {
        sb1 += "_";
        // append character of the string for which the index is changing to the corresponding stringbuilder
        // in this case, j is changing, hence we append sb2 with string b's character at j
        sb2 += seq2[j - 1];
        j--;
    }
    reverse(sb1.begin(), sb1.end());
    reverse(sb2.begin(), sb2.end());
}

int main(int argc, char* argv[]) {
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    // Read File from the input.txt;
    string base1, base2;
    string line;
    ifstream myfile(argv[1]);
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

    // Find the best alignment
    int gap = 30;
    int len1 = seq1.length();
    int len2 = seq2.length();
    int minValue = 0;
    int** opt = new int*[len1 + 1];
    for (int i = 0; i <= len1; i++) opt[i] = new int[len2 + 1];
    for (int i = 0; i <= len1; i++) opt[i][0] = gap * i;
    for (int j = 0; j <= len2; j++) opt[0][j] = gap * j;
    string align_seq1 = "";
    string align_seq2 = "";
    alignment(seq1, seq2, opt, align_seq1, align_seq2);
    // Please call getTotalMemory() only after calling your solution
    // function. It calculates max memory used by the program.
    double totalmemory = getTotalMemory();
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double totaltime = seconds * 1000 + microseconds * 1e-3;
    /*cout << "Cost: " << opt[len1][len2] << endl;
    cout << "seq1: " << align_seq1 << endl;
    cout << "seq2: " << align_seq2 << endl;
    cout << "The total time cost: " << totaltime << "sec" << endl;
    cout << "The total memory cost: " << totalmemory / 1024 << "KiB" << endl;*/

    // Ouput the result
    ofstream outputfile(argv[2]);
    outputfile << opt[len1][len2] << endl;
    outputfile << align_seq1 << endl;
    outputfile << align_seq2 << endl;
    outputfile << totaltime << "sec" << endl;
    outputfile << totalmemory / 1024 << "KiB" << endl;

    // Using Bash file to generate the plot for the Execution Time and MemoryUsage for the input point
    // Please uncomment the following code then running sudo sh generate_point.sh
    /* ofstream PlotBasic(argv[2], fstream::app);
    int totalSize = seq1.size() + seq2.size();
    PlotBasic << totalSize << "," << totaltime << "," << totalmemory / 1024 << endl;
    return 0; */
}
