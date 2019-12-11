# COSE322
고려대학교 학부 시스템프로그래밍 프로젝트



### Project 1

-----

- ext4, nilfs2 파일 시스템 profiling
- VirtualBox에서 ubuntu 16.04 이용
- 과제 명세 따라가면 잘 실행 가능





### Project 2 (Warm-up)

---

- 서버-클라이언트 간 소켓 통신 예제
- 서버와 클라이언트 모두 VirtualBox에서 ubuntu 16.04 이용
- 서버에서 패킷을 송신, 클라이언트에서 패킷을 수신
- 클라이언트 측에서는 pthread를 사용하여 패킷을 수신
- 수신한 패킷에 대한 정보와 수신 시간을 log에 적음
- proj2/proj2-warmup/log/의 파일 이름은 포트 번호이고, 그 포트로 들어온 데이터를 기록
- 서버는 주어지고, 클라이언트 코드를 짜는 것



##### 실행 방법 (proj2/proj2-warmup에서)

```bash
$ mkdir log
$ ./client
```

##### 코드를 수정한 경우

```bash
$ gcc client.c -o client -lpthread
$ ./client
```

##### input.txt에 서버 ip를 적어서 실행하는 경우

```bash
$ vim input.txt
```

​	서버 ip 입력 후,

```bash
$ ./client < input.txt
```

##### 주의사항

- 실행하려면 virtualbox의 서버와 클라이언트 vm의 <b>설정->네트워크->다음에 연결됨</b>에서 NAT 말고 호스트 전용 어댑터로 바꿔줘야 함

### Project 2

---
