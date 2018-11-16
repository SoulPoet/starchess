#include <cstdio>
#include <cstring>
#include <iostream>
#include <pthread.h>
using namespace std;

const int FIRSTPLAY = -1, VALUETABLE[] = {50, 200, 2000, 1000000};
const int hellMAXSTEP[] = {10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 15, 16, 17, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const int veryhardMAXSTEP[] = {9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 14, 15, 16, 17, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const int hardMAXSTEP[] = {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 15, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const int easyMAXSTEP[] = {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 10, 11, 12, 12, 12, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const int veryeasyMAXSTEP[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
int maxstep, stepCnt, a[80], c[8], ComVal[8];
int FPM, SPM; //FPM = First Player Mode, SPM = Seconde Player Mode

char stepstr[50];
long long CalCnt;
struct pthread_para {
	int idx;
	int isComputer;
};

inline void debug(int t1) {printf("Debug:%d\n", t1);}
inline void debug(int t1, int t2) {printf("Debug:%d %d\n", t1, t2);}
inline void debug(int t1, int t2, int t3) {printf("Debug:%d %d %d\n", t1, t2, t3);}

void init() {
	memset(a, 0, sizeof(a));
	memset(c, 0, sizeof(c));
	memset(stepstr, 0, sizeof(stepstr));
	stepCnt = 0;
}

inline void display() {
	printf("On step %d:\n", stepCnt);
	for(int i = 6; i; --i) {
		for(int j = 1; j < 8; ++j)
			if(a[(j << 3) | i] == FIRSTPLAY) printf("* ");
			else if(a[(j << 3) | i] == -FIRSTPLAY) printf("@ ");
			else printf("  ");
		puts("");
	}
	puts("");
}

void readOriginalChessborad() {
	printf("Please input the original chessborad (end with 0):");
	int tmp = 1, whoTurn = FIRSTPLAY;
	while(tmp) {
		scanf("%d", &tmp);
		if(tmp) {
			stepstr[stepCnt++] = '0' + tmp;
			++c[tmp];
			a[(tmp << 3) | c[tmp]] = whoTurn;
			whoTurn = -whoTurn;
		}
	}
	if(stepCnt) display();
}

void readPlayMode() {
	printf("Please input the first player mode (1 for Human and 0 for Computer):");
	scanf("%d", &FPM);
	printf("Please input the second player mode (1 for Human and 0 for Computer):");
	scanf("%d", &SPM);
}

void readComputerMode() {
	if(FPM && SPM) return; // If two players are all human.
	int CM; //CM = Computer Mode
	printf("Please input the computer mode (0~3):");
	scanf("%d", &CM);
	if(CM == 4) maxstep = hellMAXSTEP[stepCnt];
	else if(CM == 3) maxstep = veryhardMAXSTEP[stepCnt];
	else if(CM == 2) maxstep = hardMAXSTEP[stepCnt];
	else if(CM) maxstep = easyMAXSTEP[stepCnt];
	else maxstep = veryeasyMAXSTEP[stepCnt];
}

bool checkWin_x1(const int& p, const int& whoTurn, int aa[]) {
	int p_tmp, cnt = 0;
	for(p_tmp = p + 9; aa[p_tmp] == whoTurn; p_tmp += 9) ++cnt;
	if(cnt > 2) return true;
	cnt = 0;
	for(p_tmp = p + 7; aa[p_tmp] == whoTurn; p_tmp += 7) ++cnt;
	if(cnt > 2) return true;
	if(aa[p + 8] == whoTurn && aa[p + 16] == whoTurn && aa[p + 24] == whoTurn) return true;
	return (p & 7) > 3 && aa[p - 1] == whoTurn && aa[p - 2] == whoTurn && aa[p - 3] == whoTurn;
}

/*
bool checkWin_x2(int p, int whoTurn, int aa[100]) {
	int p_tmp, cnt = 0;
	if(aa[p - 9] == whoTurn) ++cnt;
	for(p_tmp = p + 9; aa[p_tmp] == whoTurn; p_tmp += 9) ++cnt;
	if(cnt > 2) return true;
	if(aa[p - 7] == whoTurn) cnt = 1;
	else cnt = 0;
	for(p_tmp = p - 7; aa[p_tmp] == whoTurn; p_tmp -= 7) ++cnt;
	if(cnt > 2) return true;
	//if(aa[p - 8] == whoTurn && a[p + 8] == whoTurn && aa[p + 16] == whoTurn) return true;
	return (p & 7) > 3 && aa[p - 1] == whoTurn && aa[p - 2] == whoTurn && aa[p - 3] == whoTurn;
}

bool checkWin_x6(int p, int whoTurn) {
	int p_tmp, cnt = 0;
	for(p_tmp = p - 9; a[p_tmp] == whoTurn; p_tmp -= 9) ++cnt;
	if(a[p + 9] == whoTurn) ++cnt;
	if(cnt > 2) return true;
	if(a[p + 7] == whoTurn) cnt = 1;
	else cnt = 0;
	for(p_tmp = p - 7; a[p_tmp] == whoTurn; p_tmp -= 7) ++cnt;
	if(cnt > 2) return true;
	//if(a[p - 8] == whoTurn && a[p - 16] == whoTurn && a[p - 24] == whoTurn) return true;
	return (p & 7) > 3 && a[p - 1] == whoTurn && a[p - 2] == whoTurn && a[p - 3] == whoTurn;
}
*/

bool checkWin_x7(int p, const int& whoTurn, int aa[]) {
	int p_tmp, cnt = 0;
	for(p_tmp = p - 9; aa[p_tmp] == whoTurn; p_tmp -= 9) ++cnt;
	if(cnt > 2) return true;
	cnt = 0;
	for(p_tmp = p - 7; aa[p_tmp] == whoTurn; p_tmp -= 7) ++cnt;
	if(cnt > 2) return true;
	if(aa[p - 8] == whoTurn && aa[p - 16] == whoTurn && aa[p - 24] == whoTurn) return true;
	return (p & 7) > 3 && aa[p - 1] == whoTurn && aa[p - 2] == whoTurn && aa[p - 3] == whoTurn;
}

inline bool checkWin(int p, const int& whoTurn, int aa[]) {
	int p_tmp, cnt = 0;
	for(p_tmp = p - 9; aa[p_tmp] == whoTurn; p_tmp -= 9) ++cnt;
	for(p_tmp = p + 9; aa[p_tmp] == whoTurn; p_tmp += 9) ++cnt;
	if(cnt > 2) return true;
	cnt = 0;
	for(p_tmp = p - 7; aa[p_tmp] == whoTurn; p_tmp -= 7) ++cnt;
	for(p_tmp = p + 7; aa[p_tmp] == whoTurn; p_tmp += 7) ++cnt;
	if(cnt > 2) return true;
	cnt = 0;
	for(p_tmp = p - 8; aa[p_tmp] == whoTurn; p_tmp -= 8) ++cnt;
	for(p_tmp = p + 8; aa[p_tmp] == whoTurn; p_tmp += 8) ++cnt;
	if(cnt > 2) return true;
	return (p & 7) > 3 && aa[p - 1] == whoTurn && aa[p - 2] == whoTurn && aa[p - 3] == whoTurn;
}

inline bool HasValue_x1(int p, int whoTurn, int aa[]) {
	if(aa[p] == whoTurn) return true;
	whoTurn = -whoTurn;
	if(aa[p] == whoTurn) return false;
	--p;
	if(!aa[p] && (p & 7) && checkWin_x1(p, whoTurn, aa)) return false;
	return true;
}

inline bool HasValue_x7(int p, int whoTurn, int aa[]) {
	if(aa[p] == whoTurn) return true;
	whoTurn = -whoTurn;
	if(aa[p] == whoTurn) return false;
	--p;
	if(!aa[p] && (p & 7) && checkWin_x7(p, whoTurn, aa)) return false;
	return true;
}

inline bool HasValue(int p, int whoTurn, int aa[]) {
	if(aa[p] == whoTurn) return true;
	whoTurn = -whoTurn;
	if(aa[p] == whoTurn) return false;
	--p;
	if(!aa[p] && (p & 7) && checkWin(p, whoTurn, aa)) return false;
	return true;
}

int ValueCnt(int x, int y, int whoTurn, int has_value[], int aa[]) {
	int rst = 0, cnt = 0, p = (x << 3) | y; //Compress the state
	switch(x) {
	case 1:
		if(has_value[p + 8] == -1) has_value[p + 8] = HasValue(p + 8, whoTurn, aa);
		if(has_value[p + 16] == -1) has_value[p + 16] = HasValue(p + 16, whoTurn, aa);
		if(has_value[p + 24] == -1) has_value[p + 24] = HasValue(p + 24, whoTurn, aa);
		if(has_value[p + 8] && has_value[p + 16] && has_value[p + 24])
			rst += VALUETABLE[(aa[p + 8] == whoTurn) + (aa[p + 16] == whoTurn) + (aa[p + 24] == whoTurn)];
		break;
	case 2:
		if(has_value[p - 8] == -1) has_value[p - 8] = HasValue_x1(p - 8, whoTurn, aa);
		if(has_value[p + 8] == -1) has_value[p + 8] = HasValue(p + 8, whoTurn, aa);
		if(has_value[p + 16] == -1) has_value[p + 16] = HasValue(p + 16, whoTurn, aa);
		if(has_value[p + 24] == -1) has_value[p + 24] = HasValue(p + 24, whoTurn, aa);
		if(has_value[p + 8] && has_value[p + 16]) {
			if(has_value[p - 8]) rst += VALUETABLE[(aa[p - 8] == whoTurn) + (aa[p + 8] == whoTurn) + (aa[p + 16] == whoTurn)];
			if(has_value[p + 24]) rst += VALUETABLE[(aa[p + 8] == whoTurn) + (aa[p + 16] == whoTurn) + (aa[p + 24] == whoTurn)];
		}
		break;
	case 3:
		if(has_value[p - 16] == -1) has_value[p - 16] = HasValue_x1(p - 16, whoTurn, aa);
		if(has_value[p - 8] == -1) has_value[p - 8] = HasValue(p - 8, whoTurn, aa);
		if(has_value[p + 8] == -1) has_value[p + 8] = HasValue(p + 8, whoTurn, aa);
		if(has_value[p + 16] == -1) has_value[p + 16] = HasValue(p + 16, whoTurn, aa);
		if(has_value[p + 24] == -1) has_value[p + 24] = HasValue(p + 24, whoTurn, aa);
		if(has_value[p + 8]) {
			if(has_value[p - 16] && has_value[p - 8])
				rst += VALUETABLE[(aa[p - 16] == whoTurn) + (aa[p - 8] == whoTurn) + (aa[p + 8] == whoTurn)];
			if(has_value[p - 8] && has_value[p + 16])
				rst += VALUETABLE[(aa[p - 8] == whoTurn) + (aa[p + 8] == whoTurn) + (aa[p + 16] == whoTurn)];
			if(has_value[p + 16] && has_value[p + 24])
				rst += VALUETABLE[(aa[p + 8] == whoTurn) + (aa[p + 16] == whoTurn) + (aa[p + 24] == whoTurn)];
		}
		break;
	case 4:
		if(has_value[p - 24] == -1) has_value[p - 24] = HasValue_x1(p - 24, whoTurn, aa);
		if(has_value[p - 16] == -1) has_value[p - 16] = HasValue(p - 16, whoTurn, aa);
		if(has_value[p - 8] == -1) has_value[p - 8] = HasValue(p - 8, whoTurn, aa);
		if(has_value[p + 8] == -1) has_value[p + 8] = HasValue(p + 8, whoTurn, aa);
		if(has_value[p + 16] == -1) has_value[p + 16] = HasValue(p + 16, whoTurn, aa);
		if(has_value[p + 24] == -1) has_value[p + 24] = HasValue_x7(p + 24, whoTurn, aa);
		if(has_value[p - 24] && has_value[p - 16] && has_value[p - 8])
			rst += VALUETABLE[(aa[p - 24] == whoTurn) + (aa[p - 16] == whoTurn) + (aa[p - 8] == whoTurn)];
		if(has_value[p - 16] && has_value[p - 8] && has_value[p + 8])
			rst += VALUETABLE[(aa[p - 16] == whoTurn) + (aa[p - 8] == whoTurn) + (aa[p + 8] == whoTurn)];
		if(has_value[p - 8] && has_value[p + 8] && has_value[p + 16])
			rst += VALUETABLE[(aa[p - 8] == whoTurn) + (aa[p + 8] == whoTurn) + (aa[p + 16] == whoTurn)];
		if(has_value[p + 8] && has_value[p + 16] && has_value[p + 24])
			rst += VALUETABLE[(aa[p + 8] == whoTurn) + (aa[p + 16] == whoTurn) + (aa[p + 24] == whoTurn)];
		break;
	case 5:
		if(has_value[p - 24] == -1) has_value[p - 24] = HasValue(p - 24, whoTurn, aa);
		if(has_value[p - 16] == -1) has_value[p - 16] = HasValue(p - 16, whoTurn, aa);
		if(has_value[p - 8] == -1) has_value[p - 8] = HasValue(p - 8, whoTurn, aa);
		if(has_value[p + 8] == -1) has_value[p + 8] = HasValue(p + 8, whoTurn, aa);
		if(has_value[p + 16] == -1) has_value[p + 16] = HasValue_x7(p + 16, whoTurn, aa);
		if(has_value[p - 8]) {
			if(has_value[p - 24] && has_value[p - 16])
				rst += VALUETABLE[(aa[p - 24] == whoTurn) + (aa[p - 16] == whoTurn) + (aa[p - 8] == whoTurn)];
			if(has_value[p - 16] && has_value[p + 8])
				rst += VALUETABLE[(aa[p - 16] == whoTurn) + (aa[p - 8] == whoTurn) + (aa[p + 8] == whoTurn)];
			if(has_value[p + 8] && has_value[p + 16])
				rst += VALUETABLE[(aa[p - 8] == whoTurn) + (aa[p + 8] == whoTurn) + (aa[p + 16] == whoTurn)];
		}
		break;
	case 6:
		if(has_value[p - 24] == -1) has_value[p - 24] = HasValue(p - 24, whoTurn, aa);
		if(has_value[p - 16] == -1) has_value[p - 16] = HasValue(p - 16, whoTurn, aa);
		if(has_value[p - 8] == -1) has_value[p - 8] = HasValue(p - 8, whoTurn, aa);
		if(has_value[p + 8] == -1) has_value[p + 8] = HasValue_x7(p + 8, whoTurn, aa);
		if(has_value[p - 16] && has_value[p - 8]) {
			if(has_value[p - 24]) rst += VALUETABLE[(aa[p - 24] == whoTurn) + (aa[p - 16] == whoTurn) + (aa[p - 8] == whoTurn)];
			if(has_value[p + 8]) rst += VALUETABLE[(aa[p - 16] == whoTurn) + (aa[p - 8] == whoTurn) + (aa[p + 8] == whoTurn)];
		}
		break;
	case 7:
		if(has_value[p - 24] == -1) has_value[p - 24] = HasValue(p - 24, whoTurn, aa);
		if(has_value[p - 16] == -1) has_value[p - 16] = HasValue(p - 16, whoTurn, aa);
		if(has_value[p - 8] == -1) has_value[p - 8] = HasValue(p - 8, whoTurn, aa);
		if(has_value[p - 24] && has_value[p - 16] && has_value[p - 8])
			rst += VALUETABLE[(aa[p - 24] == whoTurn) + (aa[p - 16] == whoTurn) + (aa[p - 8] == whoTurn)];
		break;
	}
	if(x > 3 && y > 3) {
		if(has_value[p - 27] == -1) has_value[p - 27] = HasValue(p - 27, whoTurn, aa);
		if(has_value[p - 18] == -1) has_value[p - 18] = HasValue(p - 18, whoTurn, aa);
		if(has_value[p - 9] == -1) has_value[p - 9] = HasValue(p - 9, whoTurn, aa);
		if(has_value[p - 27] && has_value[p - 18] && has_value[p - 9])
			rst += VALUETABLE[(aa[p - 27] == whoTurn) + (aa[p - 18] == whoTurn) + (aa[p - 9] == whoTurn)];
	}
	if(x > 2 && x < 7 && y > 2 && y < 6) {
		if(has_value[p - 18] == -1) has_value[p - 18] = HasValue(p - 18, whoTurn, aa);
		if(has_value[p - 9] == -1) has_value[p - 9] = HasValue(p - 9, whoTurn, aa);
		if(has_value[p + 9] == -1) has_value[p + 9] = HasValue(p + 9, whoTurn, aa);
		if(has_value[p - 18] && has_value[p - 9] && has_value[p + 9])
			rst += VALUETABLE[(aa[p - 18] == whoTurn) + (aa[p - 9] == whoTurn) + (aa[p + 9] == whoTurn)];
	}
	if(x > 1 && x < 6 && y > 1 && y < 5) {
		if(has_value[p - 9] == -1) has_value[p - 9] = HasValue(p - 9, whoTurn, aa);
		if(has_value[p + 9] == -1) has_value[p + 9] = HasValue(p + 9, whoTurn, aa);
		if(has_value[p + 18] == -1) has_value[p + 18] = HasValue(p + 18, whoTurn, aa);
		if(has_value[p - 9] && has_value[p + 9] && has_value[p + 18])
			rst += VALUETABLE[(aa[p - 9] == whoTurn) + (aa[p + 9] == whoTurn) + (aa[p + 18] == whoTurn)];
	}
	if(x < 5 && y < 4) {
		if(has_value[p + 9] == -1) has_value[p + 9] = HasValue(p + 9, whoTurn, aa);
		if(has_value[p + 18] == -1) has_value[p + 18] = HasValue(p + 18, whoTurn, aa);
		if(has_value[p + 27] == -1) has_value[p + 27] = HasValue(p + 27, whoTurn, aa);
		if(has_value[p + 9] && has_value[p + 18] && has_value[p + 27])
			rst += VALUETABLE[(aa[p + 9] == whoTurn) + (aa[p + 18] == whoTurn) + (aa[p + 27] == whoTurn)];
	}
	if(x > 3 && y < 4) {
		if(has_value[p - 21] == -1) has_value[p - 21] = HasValue(p - 21, whoTurn, aa);
		if(has_value[p - 14] == -1) has_value[p - 14] = HasValue(p - 14, whoTurn, aa);
		if(has_value[p - 7] == -1) has_value[p - 7] = HasValue(p - 7, whoTurn, aa);
		if(has_value[p - 21] && has_value[p - 14] && has_value[p - 7])
			rst += VALUETABLE[(aa[p - 21] == whoTurn) + (aa[p - 14] == whoTurn) + (aa[p - 7] == whoTurn)];
	}
	if(x > 2 && x < 7 && y > 1 && y < 5) {
		if(has_value[p - 14] == -1) has_value[p - 14] = HasValue(p - 14, whoTurn, aa);
		if(has_value[p - 7] == -1) has_value[p - 7] = HasValue(p - 7, whoTurn, aa);
		if(has_value[p + 7] == -1) has_value[p + 7] = HasValue(p + 7, whoTurn, aa);
		if(has_value[p - 14] && has_value[p - 7] && has_value[p + 7])
			rst += VALUETABLE[(aa[p - 14] == whoTurn) + (aa[p - 7] == whoTurn) + (aa[p + 7] == whoTurn)];
	}
	if(x > 1 && x < 6 && y > 2 && y < 6) {
		if(has_value[p - 7] == -1) has_value[p - 7] = HasValue(p - 7, whoTurn, aa);
		if(has_value[p + 7] == -1) has_value[p + 7] = HasValue(p + 7, whoTurn, aa);
		if(has_value[p + 14] == -1) has_value[p + 14] = HasValue(p + 14, whoTurn, aa);
		if(has_value[p - 7] && has_value[p + 7] && has_value[p + 14])
			rst += VALUETABLE[(aa[p - 7] == whoTurn) + (aa[p + 7] == whoTurn) + (aa[p + 14] == whoTurn)];
	}
	if(x < 5 && y > 3) {
		if(has_value[p + 7] == -1) has_value[p + 7] = HasValue(p + 7, whoTurn, aa);
		if(has_value[p + 14] == -1) has_value[p + 14] = HasValue(p + 14, whoTurn, aa);
		if(has_value[p + 21] == -1) has_value[p + 21] = HasValue(p + 21, whoTurn, aa);
		if(has_value[p + 7] && has_value[p + 14] && has_value[p + 21])
			rst += VALUETABLE[(aa[p + 7] == whoTurn) + (aa[p + 14] == whoTurn) + (aa[p + 21] == whoTurn)];
	}
	for(--p; aa[p] == whoTurn; --p) ++cnt;
	if(cnt + 4 > y) rst += VALUETABLE[cnt];
	return rst;
}

bool PutChess(int x, int whoTurn) {
	stepstr[stepCnt++] = '0' + x;
	++c[x];
	a[(x << 3) | c[x]] = whoTurn;
	display();
	return checkWin((x << 3) | c[x], whoTurn, a);
}

bool PutChess_Com(int x, int whoTurn) {
	printf("CalCnt = %lld and Chess on %d\n", CalCnt, x);
	return PutChess(x, whoTurn);
}

int dfs(int whoTurn, int step, int aa[], int cc[8], int has_value[]) {
	++CalCnt;
	int i, rst = -100000000;
	memset(has_value, -1, sizeof(int) * 80);
	if(step < maxstep) {
		int tmp[8];
		for(i = 1; i < 8; ++i)
			if(cc[i] < 6) {
				tmp[i] = ValueCnt(i, cc[i] + 1, whoTurn, has_value, aa);
				if(tmp[i] >= 1000000) return tmp[i];
			}
		for(i = 1; i < 8; ++i)
			if(cc[i] < 6) {
				++cc[i];
				aa[(i << 3) | cc[i]] = whoTurn;
				rst = max(rst, tmp[i] - dfs(-whoTurn, step + 1, aa, cc, has_value) * 199 / 200);
				aa[(i << 3) | cc[i]] = 0;
				--cc[i];
				if(rst > 800000) return rst;
			}
	} else if(step == maxstep) {
		for(i = 1; i < 8; ++i)
			if(cc[i] < 6) {
				rst = max(rst, ValueCnt(i, cc[i] + 1, whoTurn, has_value, aa));
				if(rst >= 1000000) return rst;
			}
	}  else return 0;
	return rst;
}

void* pthread_work(void* argv) {
	pthread_para *pp = (pthread_para*)argv;
	int i = pp->idx, aa[80], cc[8], isCom = pp->isComputer, has_value[80];
	memcpy(aa, a, sizeof(a));
	memcpy(cc, c, sizeof(c));
	if(cc[i] < 6) {
		++cc[i];
		aa[(i << 3) | cc[i]] = isCom;
		ComVal[i] -= dfs(-isCom, 2, aa, cc, has_value);
		debug(i, ComVal[i]);
	}
	return NULL;
}

bool turn_to_Com(int isCom) {
	int has_value[80];
	memset(has_value, -1, sizeof(int) * 80);
	CalCnt = 0;
	if(stepCnt > 1) {
		for(int i = 1; i < 8; ++i)
			if(c[i] < 6) {
				ComVal[i] = ValueCnt(i, c[i] + 1, isCom, has_value, a);
				if(ComVal[i] >= 1000000) return PutChess_Com(i, isCom);
			}
		int bestX, bestD = -100000000;
		pthread_para pp[10];
		pthread_t tid[10];
		for(int i = 1; i < 8; ++i) {
			pp[i].isComputer = isCom;
			pp[i].idx = i;
			pthread_create(tid + i, NULL, pthread_work, pp + i);
		}
		pthread_join(tid[1], NULL);
		pthread_join(tid[2], NULL);
		pthread_join(tid[3], NULL);
		pthread_join(tid[4], NULL);
		pthread_join(tid[5], NULL);
		pthread_join(tid[6], NULL);
		pthread_join(tid[7], NULL);
		for(int i = 1; i < 8; ++i) {
			if(c[i] < 6) {
				if(ComVal[i] > bestD) {
					bestD = ComVal[i];
					bestX = i;
				}
				// debug(i, ComVal[i]);
			}
		}
		return PutChess_Com(bestX, isCom);
	} else if(c[4]) return PutChess_Com(3, isCom);
	return PutChess_Com(4, isCom);
}

bool turn_to_Human(int isHuman) {
	int tmp = 0;
	while(tmp < 1 || tmp > 7) {
		scanf("%d", &tmp);
		if(!tmp) puts(stepstr);
		else if(tmp == 8) return turn_to_Com(isHuman);
	}
	return PutChess(tmp, isHuman);
}

void onPlay() {
	bool HasWin = false;
	while(stepCnt < 42)
		if(stepCnt & 1) {
			if(SPM ? turn_to_Human(-FIRSTPLAY) : turn_to_Com(-FIRSTPLAY)) {
				puts("Player2 Win!");
				HasWin = true;
				break;
			}
		} else {
			if(FPM ? turn_to_Human(FIRSTPLAY) : turn_to_Com(FIRSTPLAY)) {
				puts("Player1 Win!");
				HasWin = true;
				break;
			}
		}
	if(!HasWin) puts("Draw!");
	puts(stepstr);
}

int main() {
	init();
	readOriginalChessborad();
	readPlayMode();
	readComputerMode();
	onPlay();
	return 0;
}