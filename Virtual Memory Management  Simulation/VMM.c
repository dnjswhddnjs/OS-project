#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>

const int MIN = 0;
const int FIFO = 1;
const int LRU = 2;
const int LFU = 3;
const int WS = 4;
const int INF = 99999;


int N=0;
int M = 0;
int W = 0;
int K = 0;

int P_page[5][21] = { 0, };
int FIFO_idx = 0;
int Page_Freq[101] = { 0, };
int Page_Fault[5] = { 0, };
bool Working_Set[101] = { false, };

int NumberOfAllocated = 0;
int Total_Allocated = 0;
int R[1001] = { 0, };
int PF_Pos = 0;
int Ref_Page = 0;
int Time = 0;	//Time은 1부터 시작

bool isPagefault(int algo,int Ref_Page) {
	for (int i = 1; i <= M; i++) {
		if (P_page[algo][i] == Ref_Page) return false;
	}
	return true;
}

bool isPagefull(int algo) {
	
	for (int i = 1; i <= M; i++) {
		if (P_page[algo][i] == -1) return false;
	}
	return true;

}

int Find_Df(int page) {
	for (int fTime = Time+1; fTime <= K; fTime++) {
		if (R[fTime] == page) return fTime-Time;
	}
	return INF;
}

int Find_Db(int page) {
	for (int bTime = Time - 1; bTime >= 1; bTime--) {
		if (R[bTime] == page) return Time-bTime;
	}
	printf("FInd_Db_ERROR\n");
	return -1;
}

void Find_Working_Set(int Ref_Page){
	for (int i = 0; i <= 100; i++) {
		Working_Set[i] = false;
	}

	for (int i = Time; i >= Time - W && i>=1; i--) {
		if (R[i] < 0) printf("Find_Working_Set ERROR\n");
		Working_Set[R[i]] = true;
	}
	Working_Set[Ref_Page] = true;

}

int Find_Victim(int algo) {
	int victim_pos=0;

	if (algo == MIN) {
		int Df[21] = { 0, };
		int MAX_Df = 0;
		for (int i = 1; i <= M; i++) {
			Df[i] = Find_Df(P_page[MIN][i]);
			if (MAX_Df != INF && MAX_Df == Df[i])printf("FInd_Victim_MIN ERROR\n");
			if (MAX_Df < Df[i]) {	//tie-breaking : page frame 번호가 작은 것(즉 메모리 상에 더 앞선 주소에 위치한 것)
				MAX_Df = Df[i];
				victim_pos = i;
			}
		}
	}
	else if (algo == FIFO) {

		FIFO_idx = (FIFO_idx % M) + 1;
		
		victim_pos = FIFO_idx;


	}
	
	else if (algo == LRU) {
		int Db[21] = { 0, };
		int MAX_Db = 0;
		for (int i = 1; i <= M; i++) {
			Db[i] = Find_Db(P_page[LRU][i]);
			if (MAX_Db < Db[i]) {	
				MAX_Db = Db[i];
				victim_pos = i;
			}
		}
		if (MAX_Db == 0) printf("FInd_Victim_LRU ERROR\n");
	}
	else if (algo == LFU) {
		int MIN_freq = INF;
		for (int i = 1; i <= M; i++) {	//tie-breaking : page frame 번호가 작은 것(즉 메모리 상에 더 앞선 주소에 위치한 것)
			if (MIN_freq > Page_Freq[P_page[LFU][i]]) {
				MIN_freq = Page_Freq[P_page[LFU][i]];
				victim_pos = i;
			}
		}
	}
	else if (algo == WS) {		//WS의 경우 replace 방법으로 LRU 사용
		int Db[21] = { 0, };
		int MAX_Db = 0;
		for (int i = 1; i <= M; i++) {
			Db[i] = Find_Db(P_page[WS][i]);
			if (MAX_Db < Db[i]) {
				MAX_Db = Db[i];
				victim_pos = i;
			}
		}
		NumberOfAllocated--;
		if (MAX_Db == 0) printf("FInd_Victim_LRU ERROR\n");
	}
	else {
		printf("FInd_Victim_algo ERROR\n");
	}


	return victim_pos;
}

int Find_Load_Pos(int algo) {

	for (int i = 1; i <= 100; i++) {
		if (P_page[algo][i] == -1) return i;
	}

	printf("ERROR\n");
	return -1;
}



void Load(int algo,int load_pos,int Ref_Page) {

	if (algo == WS) NumberOfAllocated++;

	P_page[algo][load_pos] = Ref_Page;
	PF_Pos = load_pos;
}



void Show_Memory_State(int algo,int Time,bool PF) {
	printf("Time : %d  / ", Time);
	printf("Ref. string : %d\n", R[Time]);

	if (PF)printf("Page Fault\n");
	else printf("not in Page Fault\n");

	if (algo == WS) {
		printf("Working Set : {");
		for (int i = 0; i <= N; i++) {
			if (Working_Set[i] == true) printf(" %d",i);
		}
		printf(" }\n");
	}

	printf("<Memory state>\n");
	for (int i = 1; i <= M; i++) {
		printf("page frame %d : ", i);
		if (P_page[algo][i] == -1) printf("-");
		else printf("%d", P_page[algo][i]);
		
		if (PF_Pos == i) printf(" <-");
		printf("\n");
		
	}
	printf("\n");

}

int main(char argc, char* argv[]) {

	for (int algo = 0; algo <= 4; algo++) {
		for (int i = 0; i <= 20; i++) {
			P_page[algo][i] = -1;
		}
	}
	

	if (!freopen(argv[1], "r", stdin)) {
		printf("ERROR");
		return 0;
	};

	
	scanf("%d", &N);
	scanf("%d", &M);
	scanf("%d", &W);
	scanf("%d", &K);

	for (int i = 1; i <= K; i++) {
		scanf("%d", &R[i]);
	}


	for (int algo = MIN; algo <= WS; algo++) {
		Time = 0;
		FIFO_idx = 0;

		printf("\n------------------Algorithm: ");
		if (algo == MIN) printf("MIN");
		else if (algo == FIFO) printf("FIFO");
		else if (algo == LRU) printf("LRU");
		else if (algo == LFU) printf("LFU");
		else if (algo == WS) printf("WS");
		else  printf("main_ERROR");
		printf("------------------\n\n");

		
		for (int t = 1; t <= K; t++) {
			bool PF = false;
			PF_Pos = -1;
			Ref_Page = R[t];
			Time++;
			if(algo==LFU) Page_Freq[Ref_Page]++;

			if (algo == WS) {		//나가는 page
				Find_Working_Set(Ref_Page);
				for (int i = 1; i <= M; i++) {
					if (P_page[WS][i] < 0) continue;
					if (!Working_Set[P_page[WS][i]]) {
						P_page[WS][i] = -1;
						NumberOfAllocated--;
					}
				}
			}

			if (!isPagefault(algo, Ref_Page)) {
				if (algo == WS) Total_Allocated += NumberOfAllocated;
				Show_Memory_State(algo, Time,PF);				
				continue;
			}

			PF = true;	//pagefault  발생!
			Page_Fault[algo]++;
			

			int load_pos = 0;
			if (isPagefull(algo)) {
				
				load_pos = Find_Victim(algo);

				Load(algo, load_pos, Ref_Page);
			}
			else {
				

				load_pos = Find_Load_Pos(algo);
				Load(algo, load_pos, Ref_Page);
			}

			Show_Memory_State(algo,Time,PF);
			if (algo == WS) {
				Total_Allocated += NumberOfAllocated;
			
			}
				
			
		}

		printf("the number of Page Fault : %d\n", Page_Fault[algo]);
		

		if(algo==WS) printf("the avg. number of frames allocated : %.2f", (double)Total_Allocated / K);
		else printf("Page Fault Rate : %.2f\n", (double)Page_Fault[algo] / K);

	}

		

	
	
}
