#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include "hmm.h"

using namespace std;

vector<pair<string, vector<int>>> dic;	// (name, vector of phones)
int nstate = 1;		// Initial state
double** t;

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

void read_dic() {
	FILE *fp = fopen("dictionary.txt", "r");

	while (!feof(fp)) {
		char line[100];
		vector<int> phones;
		fgets(line, 100, fp);
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';
		char *tokenized = strtok(line, "\t");
		string name(tokenized);
		tokenized = strtok(NULL, " ");
		do {
			phones.push_back(phone2int(string(tokenized)));
			tokenized = strtok(NULL, " ");
		} while (tokenized != NULL);
		dic.push_back(make_pair(name, phones));
	}

	t = new double*[nstate];
	for (int i = 0; i < nstate; i++) {
		t[i] = new double[nstate];
	}

	fclose(fp);
}

int main() {
	read_dic();
	for (int i = 0; i < dic.size(); i++) {
		cout << dic[i].first << " ";
		for (int j = 0; j < dic[i].second.size(); j++) {
			cout << dic[i].second[j] << " ";
		}
		cout << endl;
	}

	system("pause");
	return 0;
}
