#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define BUFFER_SIZE 10 // 유한 버퍼 사이즈 선언
#define THREAD_COUNT 2 // 수행될 쓰레드 개수 선언

int buffer[BUFFER_SIZE]; // 유한 버퍼 선언
int in = -1; // 생산자 변수
int out = -1; // 소비자 변수
int num = 0; // 최종 생산 - 소비 합
sem_t buffer_slot; // 빈 슬롯인 세마포어 변수 생성
sem_t items; // item이 있음을 의미하는 세마포어 변수 생성
sem_t mutex; // 이진 세마포어를 위한 변수 생성.

void *readers(void *arg); // 소비자 함수 선언부

void *writers(void *arg); // 생산자 함수 선언부int main(void)

int main(void)
{ // Main Func
	sem_init(&buffer_slot, 0, BUFFER_SIZE); // 세마포어 변수 초기화.
	sem_init(&items, 0, 0); // 세마포어 변수 초기화.
	sem_init(&mutex, 0, 1); // 이진 세마포어 변수 초기화
	pthread_t threads[THREAD_COUNT]; // 쓰레드 배열 선언
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		if (i % 2 == 0)
			pthread_create(&threads[i], NULL, readers, NULL);
		else
			pthread_create(&threads[i], NULL, writers, NULL);
	}
	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_join(threads[i], NULL);
		
	printf("\n result : %d \n \n", num);
	pthread_exit(NULL); // 쓰레드 종료
	sem_destroy(&buffer_slot); // 세마포어 변수 삭제
	sem_destroy(&items); // 세마포어 변수 삭제
	return 0;
}

void *readers(void *arg)
{ // 소비자 함수 구현부
	int data; // idx를 위한 지역변수
	for (int i = 0; i < BUFFER_SIZE * 10; i++)
	{
		sem_wait(&items); // 대기
		sem_wait(&mutex); // 임계 영역
		out++;
		num--;
		out %= BUFFER_SIZE;
		data = buffer[out];
		sem_post(&mutex); // 임계영역 끝
		sem_post(&buffer_slot);
		printf("Readers consume Index : %d \n", data); // 소비한 idx 출력
	}
}

void *writers(void *arg)
{ // 생산자 함수 구현부
	int i, data;
	for (int i = 0; i < BUFFER_SIZE * 10; i++)
	{
		printf("writers produce Index : %d \n", i); // 생산된 idx 출력
		sem_wait(&buffer_slot); // 대기
		sem_wait(&mutex); // 임계영역 시작
		num++;
		in++;
		in %= BUFFER_SIZE;
		buffer[in] = i;
		sem_post(&mutex); // 임계영역 끝
		sem_post(&items);
	}
}
