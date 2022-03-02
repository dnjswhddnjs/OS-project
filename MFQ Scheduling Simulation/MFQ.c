#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define q0_quantum 1
#define q1_quantum 2
#define q2_quantum 4
#define READY 1
#define ON_CPU 2
#define ON_IO 3
#define COMPLETE 4
#define MAX_PROCESSES 10000
#define EXECUTE_COMPLETE true
#define EXECUTE_INCOMPLETE false
#define CPU false
#define IO true

typedef struct {
	int front;
	int rear;
	int items[MAX_PROCESSES];
}Queue;

typedef struct {

	Queue* SequenceOfBT;
	int pid;
	int state;
	int AT;
	int Init_Q;
	int Prev_Q;
	int Cycles;
	int TT;
	int WT;
	int CurBurst;


}Process;
Process P[MAX_PROCESSES + 1];




int total_TIME = -1;
int Ps = 0;
int PonIO = 0;



//큐 정의 부분

void InitQueue(Queue* pqueue) {
	pqueue->front = pqueue->rear = -1;
}
bool isQFull(Queue* pqueue) {
	if (pqueue->rear + 1 >= MAX_PROCESSES)
		return true;
	else
		return false;
}
bool isQEmpty(Queue* pqueue) {
	if (pqueue->front == pqueue->rear)
		return true;
	else
		return false;
}
void enQueue(Queue* pqueue, int val) {
	if (isQFull(pqueue))
		printf("Queue Overflow !! \n");
	else
		pqueue->items[++pqueue->rear] = val;
}
int deQueue(Queue* pqueue) {
	if (isQEmpty(pqueue)) {
		printf("Queue Underflow !! \n");
		return -1;
	}

	else {
		//return q[++front];
		int ret = pqueue->items[++pqueue->front];
		pqueue->items[pqueue->front] = 0;
		return ret;
	}

}
int Qpeek(Queue* pqueue) {
	if (isQEmpty(pqueue)) {
		printf("Queue Underflow !! \n");
		return -1;
	}

	else
		return pqueue->items[pqueue->front + 1];
}


Queue* q0;
Queue* q1;
Queue* q2;
Queue* q3;




bool ALL_PROCESS_CLEAR = true;

void SET_RQ() {
	ALL_PROCESS_CLEAR = true;
	for (int i = 1; i <= Ps; i++) {
		if (P[i].state == 0) ALL_PROCESS_CLEAR = false;
		if (total_TIME == P[i].AT) {
			P[i].state = READY;
			if (P[i].Init_Q == 0)
				enQueue(q0, P[i].pid);
			else if (P[i].Init_Q == 1)
				enQueue(q1, P[i].pid);
			else if (P[i].Init_Q == 2)
				enQueue(q2, P[i].pid);
			else if (P[i].Init_Q == 3)
				enQueue(q3, P[i].pid);


			P[i].CurBurst = deQueue(P[i].SequenceOfBT);

			printf("\n                    %d    %3d      새로운 P\n", i, P[i].Init_Q);

		}
		else continue;

	}
}

int Search_RQ() {

	if (!isQEmpty(q0)) {
		return 0;
	}
	else if (!isQEmpty(q1)) {
		return 1;
	}
	else if (!isQEmpty(q2)) {
		return 2;
	}
	else if (!isQEmpty(q3)) {
		return 3;
	}
	else if (PonIO > 0)	//RQ가 비었으나 IO중인 프로세스가 있음
		return 4;
	else if (!ALL_PROCESS_CLEAR)	 //RQ도 비었고 IO중인 프로세스도 없지만 RQ에 진입 대기중인 프로세스가 있음
		return 5;
	else //RQ도 비었고 IO중인 프로세스도 없고, 모든 프로세스가 complete 된 상태
		return 6;


}

int Wakeup(int pid) {
	P[pid].state = READY;
	//P[pid].CPU_OR_IO = CPU; //플래그를 CPU로 변경
	P[pid].CurBurst = deQueue(P[pid].SequenceOfBT);
	PonIO--;
	if (P[pid].Prev_Q == 3) {
		enQueue(q3, pid);
		return 3;
	}
	else if (P[pid].Prev_Q == 2) {
		enQueue(q1, pid);
		return 1;
	}
	else if (P[pid].Prev_Q == 1) {
		enQueue(q0, pid);
		return 0;
	}
	else if (P[pid].Prev_Q == 0) {
		enQueue(q0, pid);
		return 0;
	}
	else {
		printf("ERROR\n");
		return -1;
	}
}

void Time_Elapse() {

	for (int i = 1; i <= Ps; i++) {
		if (P[i].state == READY) {
			if (P[i].Prev_Q == -1) {
				P[i].WT++;

			}

		}
		else if (P[i].state == ON_IO) {
			P[i].CurBurst--;
			//P[i].CurBurst.second--;
			if (P[i].CurBurst <= 0) {
				printf("\n                    %d    %3d      IO burst 만료\n", i, Wakeup(i));
			}
		}
		else if (P[i].state == ON_CPU) {

			P[i].CurBurst--;
		}
	}


}

void Record(int pid) {


	if (pid) {
		printf("         %3d", pid);
	}

	printf("\n");

}
int Preempted(int pid) {
	if (P[pid].Prev_Q == 3) {
		enQueue(q3, pid);
		return 3;
	}
	else if (P[pid].Prev_Q == 2) {
		enQueue(q3, pid);
		return 3;
	}
	else if (P[pid].Prev_Q == 1) {
		enQueue(q2, pid);
		return 2;
	}
	else if (P[pid].Prev_Q == 0) {
		enQueue(q1, pid);
		return 1;
	}
	else {
		printf("ERROR\n");
		return -1;
	}
}


int quantum_count = 0;
int CurQuantum = 0;
int EX_pid = 0;

bool KEEP_EXECUTE = false;
bool Execute() {
	total_TIME++;
	printf("---------------------------------------------------------");
	printf("\n<%d>\n", total_TIME);

	SET_RQ();
	if (KEEP_EXECUTE == false) {
		int Q = Search_RQ();
		if (Q == 0) {

			EX_pid = deQueue(q0);
			P[EX_pid].Prev_Q = 0;
			CurQuantum = q0_quantum;
		}
		else if (Q == 1) {
			EX_pid = deQueue(q1);
			P[EX_pid].Prev_Q = 1;
			CurQuantum = q1_quantum;
		}
		else if (Q == 2) {
			EX_pid = deQueue(q2);
			P[EX_pid].Prev_Q = 2;
			CurQuantum = q2_quantum;
		}
		else if (Q == 3) {

			EX_pid =deQueue(q3);
			P[EX_pid].Prev_Q = 3;
			CurQuantum = 1000000000;
		}
		else if (Q == 4 || Q == 5)
		{
			//RQ는 비었지만 IO가 진행중인 프로세스가 있음 
			//RQ도 비었고 IO가 진행중인 프로세스가 있지만 RQ에 진입 대기중인 프로세스가 있음
			Record(0);
			printf("                                  IDLE\n");
			Time_Elapse();	//WT증가,TT증가 , CPU에 있는 프로세스의 cur_BT 감소, IO 큐에 있는 프로세스들의 IO burst time 감소

			return EXECUTE_INCOMPLETE;
		}

		else if (Q == 6) {	//completed
			printf("실행 종료\n");
			return EXECUTE_COMPLETE;
		}
		else printf("ERROR\n");
		P[EX_pid].state = ON_CPU;

	}

	Time_Elapse();	//WT증가,TT증가 , CPU에 있는 프로세스의 cur_BT 감소, IO 큐에 있는 프로세스들의 IO burst time 감소







	if (P[EX_pid].CurBurst == 0)	//CPU Burst time을 다 소비한상황 
	{
		if (isQEmpty(P[EX_pid].SequenceOfBT))	//completed
		{
			P[EX_pid].state = COMPLETE;
			Record(EX_pid);
			printf("%35d 프로세스 completed\n", EX_pid);
			quantum_count = 0;
			KEEP_EXECUTE = false;
			P[EX_pid].TT = total_TIME - P[EX_pid].AT;
		}

		else	//IO burst time을 시작해야하는 상황
		{
			P[EX_pid].state = ON_IO;
			P[EX_pid].CurBurst = deQueue(P[EX_pid].SequenceOfBT);
			Record(EX_pid);
			printf("%35d 프로세스 I/O진입\n", EX_pid);
			PonIO++;
			quantum_count = 0;
			KEEP_EXECUTE = false;
		}


	}
	else {
		quantum_count++;
		if (quantum_count == CurQuantum) {	 //preemted
			Record(EX_pid);
			printf("\n                    %d    %3d      Preemption(남은 CPU BT: %d)\n", EX_pid, Preempted(EX_pid), P[EX_pid].CurBurst);
			P[EX_pid].state = READY;
			quantum_count = 0;
			KEEP_EXECUTE = false;

		}
		else {	//keep

			Record(EX_pid);
			KEEP_EXECUTE = true;


		}
	}



	return EXECUTE_INCOMPLETE;
}




int main(char argc, char* argv[]) {



	for (int i = 1; i <= MAX_PROCESSES; i++) {
		P[i].pid = 0;
		P[i].state = 0;
		P[i].AT = 0;
		P[i].Init_Q = 0;
		P[i].Prev_Q = -1;
		P[i].Cycles = 0;
		P[i].TT = 0;
		P[i].WT = 0;
		P[i].CurBurst = 0;
		P[i].SequenceOfBT = (Queue*)malloc(sizeof(Queue));
		InitQueue(P[i].SequenceOfBT);

	}








	if (!freopen(argv[1], "r", stdin)) {
		printf("ERROR");
		return 0;
	};


	scanf("%d", &Ps);

	q0 = (Queue*)malloc(sizeof(Queue));
	q1 = (Queue*)malloc(sizeof(Queue));
	q2 = (Queue*)malloc(sizeof(Queue));
	q3 = (Queue*)malloc(sizeof(Queue));
	InitQueue(q0);
	InitQueue(q1);
	InitQueue(q2);
	InitQueue(q3);



	for (int i = 1; i <= Ps; i++) {
		scanf("%d %d %d %d", &P[i].pid, &P[i].AT, &P[i].Init_Q, &P[i].Cycles);




		for (int j = 1; j <= P[i].Cycles - 1; j++) {
			int temp_CB = 0;
			int temp_IB = 0;
			scanf("%d %d", &temp_CB, &temp_IB);

			enQueue(P[i].SequenceOfBT, temp_CB);
			enQueue(P[i].SequenceOfBT, temp_IB);


		}
		int last_CB = 0;
		scanf("%d", &last_CB);
		enQueue(P[i].SequenceOfBT, last_CB);



	}


	printf("Time   실행P   추가된P   진입RQ   추가사항\n");

	while (Execute() == false);

	int Total_TT = 0;
	int Total_WT = 0;
	printf("\nPID     TT     WT\n");
	for (int i = 1; i <= Ps; i++) {
		printf("%3d     %2d     %2d\n", i, P[i].TT, P[i].WT);
		Total_TT += P[i].TT;
		Total_WT += P[i].WT;
	}
	printf("AVG.TT : %.1f\n", (float)Total_TT / Ps);
	printf("AVG.WT : %.1f\n", (float)Total_WT / Ps);


	return 0;
}
