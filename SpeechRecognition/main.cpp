#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <string>
#include <cmath>
#include <io.h>
#include "hmm.h"

using namespace std;

typedef struct {
	string name;
	int head;
	int tail;
	vector<int> phones;
} wordType;

vector<wordType> dic;	// (name, vector of phones)
int nstate = 1;		// Initial state
double **t;			// Transition probability matrix
double *in, *out;

string int2phone(int phone) {
	return phones[phone].name;
}
string int2word(int word) {
	return dic[word].name;
}
int phone2int(string phone) {
	if (phone.compare("f") == 0)
		return 0;
	else if (phone.compare("k") == 0)
		return 1;
	else if (phone.compare("n") == 0)
		return 2;
	else if (phone.compare("r") == 0)
		return 3;
	else if (phone.compare("s") == 0)
		return 4;
	else if (phone.compare("t") == 0)
		return 5;
	else if (phone.compare("v") == 0)
		return 6;
	else if (phone.compare("w") == 0)
		return 7;
	else if (phone.compare("z") == 0)
		return 8;
	else if (phone.compare("ah") == 0)
		return 9;
	else if (phone.compare("ao") == 0)
		return 10;
	else if (phone.compare("ay") == 0)
		return 11;
	else if (phone.compare("eh") == 0)
		return 12;
	else if (phone.compare("ey") == 0)
		return 13;
	else if (phone.compare("ih") == 0)
		return 14;
	else if (phone.compare("iy") == 0)
		return 15;
	else if (phone.compare("ow") == 0)
		return 16;
	else if (phone.compare("th") == 0)
		return 17;
	else if (phone.compare("uw") == 0)
		return 18;
	else if (phone.compare("sil") == 0)
		return 19;
	else if (phone.compare("sp") == 0)
		return 20;
	else
		return -1;
}
int word_count(string word) {
	int count = 0;
	for (int i = 0; i < dic.size(); i++) {
		if (word.compare(dic[i].name) == 0)
			count++;
	}
	return count;
}
int nstate_of(int phone) {
	return (phone == 20) ? 1 : 3;
}
tuple<int, int, int> state_idx(int n) {
	int state = 0;
	for (int i = 0; i < dic.size(); i++) {
		for (int j = 0; j < dic[i].phones.size(); j++) {
			for (int k = 0; k < nstate_of(dic[i].phones[j]); k++) {
				state++;
				if (n == state)
					return make_tuple(i, j, k);
			}
		}
	}

	return make_tuple(-1, -1, -1);
}
vector<int> word2int(string word) {
	vector<int> v;
	for (int i = 0; i < dic.size(); i++) {
		if (word.compare(dic[i].name) == 0)
			v.push_back(i);
	}
	return v;
}

void read_dic() {
	FILE *fp = fopen("dictionary.txt", "r");

	while (!feof(fp)) {
		wordType word;
		word.head = nstate;
		char line[100];
		fgets(line, 100, fp);
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';
		char *tokenized = strtok(line, "\t");
		word.name = string(tokenized);
		tokenized = strtok(NULL, " ");
		do {
			int phone = phone2int(string(tokenized));
			nstate += nstate_of(phone);
			word.phones.push_back(phone);
			tokenized = strtok(NULL, " ");
		} while (tokenized != NULL);
		word.tail = nstate - 1;
		dic.push_back(word);
	}

	fclose(fp);
}

void make_t() {
	t = new double*[nstate];
	for (int i = 0; i < nstate; i++) {
		t[i] = new double[nstate];
		for (int j = 0; j < nstate; j++) {
			t[i][j] = 0.0;
		}
	}
	in = new double[nstate];
	out = new double[nstate];
	for (int i = 0; i < nstate; i++) {
		in[i] = 1.0;
		out[i] = 1.0;
	}

	// word hmm ¸¸µé±â
	for (int i = 0; i < dic.size(); i++) {
		// forward
		int state = dic[i].head;
		for (int j = 0; j < dic[i].phones.size(); j++) {
			int phone = dic[i].phones[j];
			for (int k = 0; k < nstate_of(phone); k++) {
				in[state + k] *= phones[phone].tp[0][k + 1];
				if (j > 0) {
					int prev = dic[i].phones[j - 1];
					in[state + k] *= phones[prev].tp[0][nstate_of(prev) + 1];
				}

				for (int l = 0; l < nstate_of(phone); l++) {
					t[state + k][state + l] = phones[phone].tp[k + 1][l + 1];
				}
			}
			state += nstate_of(phone);
		}
		// backward
		state = dic[i].tail - nstate_of(dic[i].phones.back()) + 1;
		for (int j = dic[i].phones.size() - 1; j >= 0; j--) {
			int phone = dic[i].phones[j];
			for (int k = 0; k < nstate_of(phone); k++) {
				out[state + k] *= phones[phone].tp[k + 1][nstate_of(phone) + 1];
				if (j < dic[i].phones.size() - 1) {
					int next = dic[i].phones[j + 1];
					out[state + k] *= phones[next].tp[0][nstate_of(next) + 1];
				}
			}
			if (j > 0)
				state -= nstate_of(dic[i].phones[j - 1]);
		}
	}
}

void read_prob() {
	FILE *uni = fopen("unigram.txt", "r");

	while (!feof(uni)) {
		char buf[30];
		double prop;
		fscanf(uni, "%s %lf", buf, &prop);
		vector<int> word = word2int(string(buf));
		for (int h = 0; h < word.size(); h++) {
			for (int i = dic[word[h]].head; i <= dic[word[h]].tail; i++) {
				t[0][i] += prop / word.size() * in[i];
			}
		}
	}

	fclose(uni);

	FILE *bi = fopen("bigram.txt", "r");

	while (!feof(bi)) {
		char buf1[30];
		char buf2[30];
		double prop;
		fscanf(bi, "%s %s %lf", buf1, buf2, &prop);
		vector<int> word1 = word2int(string(buf1));
		vector<int> word2 = word2int(string(buf2));
		for (int g = 0; g < word1.size(); g++) {
			for (int h = 0; h < word2.size(); h++) {
				for (int i = dic[word1[g]].head; i <= dic[word1[g]].tail; i++) {
					for (int j = dic[word2[h]].head; j <= dic[word2[h]].tail; j++) {
						t[i][j] += prop / word1.size() / word2.size() * out[i] * in[j];
					}
				}
			}
		}
	}

	fclose(bi);
}
double normal(double x, double mean, double var) {
	return exp(-pow(x - mean, 2) / (2 * var)) / sqrt(2 * var * M_PI);
}
void viterbi(double **ob, pair<double, vector<int>> **pm, int d, int lastf) {
	int state = 0;
	for (int i = 0; i < dic.size(); i++) {
		for (int j = 0; j < dic[i].phones.size(); j++) {
			int phone = dic[i].phones[j];
			for (int k = 0; k < nstate_of(phone); k++) {
				state++;
				double sum = 0.0;
				for (int l = 0; l < N_PDF; l++) {
					pdfType pdf = phones[phone].state[k].pdf[l];
					double pi = pdf.weight;
					for (int m = 0; m < d; m++) {
						pi *= normal(ob[lastf][m], pdf.mean[m], pdf.var[m]);
					}
					sum += pi;
				}
				if (lastf == 0) {
					pm[lastf][state].first = t[0][state] + log(sum);
					pm[lastf][state].second.push_back(state);
				}
				else {
					int argmax = 0;
					for (int l = 0; l < nstate; l++) {
						double result = pm[lastf - 1][l].first + t[l][state] + log(sum);
						if (result > pm[lastf][state].first) {
							pm[lastf][state].first = result;
							argmax = l;
						}
					}
					pm[lastf][state].second.resize(pm[lastf - 1][argmax].second.size());
					copy(pm[lastf - 1][argmax].second.begin(), pm[lastf - 1][argmax].second.end(), pm[lastf][state].second.begin());
					pm[lastf][state].second.push_back(state);
				}
			}
		}
	}
}

int main() {
	read_dic();
	make_t();
	read_prob();
	
	for (int i = 0; i < nstate; i++) {
		for (int j = 0; j < nstate; j++) {
			t[i][j] = log(t[i][j]);
		}
	}

	string path = ".\\tst\\f\\ak\\44z5938.txt";
	ifstream fin(path);
	int f, d;
	fin >> f >> d;
	double **ob = new double*[f];											// Observed matrix (frame, dim)
	pair<double, vector<int>> **pm = new pair<double, vector<int>>*[f];		// Probability matrix (frame, state), vector of argmax
	for (int i = 0; i < f; i++) {
		ob[i] = new double[d];
		for (int j = 0; j < d; j++) {
			fin >> ob[i][j];
		}
		
		pm[i] = new pair<double, vector<int>>[nstate];
		for (int j = 0; j < nstate; j++) {
			pm[i][j].first = -INFINITY;
		}
	}
	for (int i = 0; i < f; i++) {
		viterbi(ob, pm, d, i);
	}
	int argmax = 0;
	double max = -INFINITY;
	for (int i = 0; i < nstate; i++) {
		if (pm[f - 1][i].first > max) {
			max = pm[f - 1][i].first;
			argmax = i;
		}
	}
	for (int i = 0; i < f; i++) {
		int state = pm[f - 1][argmax].second[i];
		
		cout << int2word(get<0>(state_idx(state))) << " ";
		cout << int2phone(dic[get<0>(state_idx(state))].phones[get<1>(state_idx(state))]);
		cout << get<2>(state_idx(state)) << endl;
	}

	system("pause");
	return 0;
}
