# Big-number-library-Project
2023 고급응용프로그래밍 과제
# 🚀 Airplane / 300km  
**동적·정적 암호 연산 라이브러리 (ECDH 기반)**  
최종 보고서 기반 README  

---

## 1. 👨‍💻 개발진 소개
- **이름** : 위승주, 이재석  
- **학년** : 3학년  
- **소속** : 암호 최적화 및 응용 연구실  
- **특이사항** : 복학 후 돌아온 귀요미들  
- **현재 연구** : GPU 암호 가속  

---

## 2. 🎯 개발 목표와 전략
- **개발 목표**  
  - 범용 환경에서 동작하는 **동적(Big Number) 연산 라이브러리**  
  - 정적(Big Number) 기반의 **ECDH 암호 라이브러리**

- **추진 전략**  
  - 라이브러리 코드는 공동 개발  
  - 랜덤값, 메모리 누수 확인, Makefile 제작 등은 분업  

---

## 3. 📅 개발 일지 요약
| 주차 | 주요 내용 |
|------|-----------|
| 1주차 | Orientation |
| 3주차 | Basic operation 동적 구현 시작 (20%), CMake 환경 구성 |
| 5주차 | 정적 Big number 구현(512bit) 시작, 동적 검증툴 구축 |
| 6주차 | AES-DRBG 도입 및 Rand 함수 구현 |
| 7~8주차 | Add/Sub, School-book 곱셈 동적·정적 구현 및 검증 |
| 9주차 | Karatsuba 곱셈 구현, Memory Leak 검사 |
| 10주차 | 정적 역원 알고리즘(FLT, EEA) 구현 |
| 11주차 | Jacobian 좌표 상 연산 구현 |
| 12주차 | 속도 비교 (School-book vs Karatsuba, FLT vs EEA) |
| 13~14주차 | ECADD, ECDBL, Barret Reduction 구현 |
| 15주차 | ECDH 로직 정적 구현, 최종 발표 및 회식 🎉 |

---

## 4. 📚 개발 라이브러리 소개

### 🔹 동적 Big Number 라이브러리
- **지원 환경** : Linux (gcc, g++), Windows (MSVC)  
- **구현 연산**  
  - ADD, SUB  
  - MUL (School book, Improved, Karatsuba)  
  - SQR, DIV(Binary), REDC(Barrett), EXP(L2R)  
- **검증 기법** : Python 기반 검증툴  

### 🔹 정적 ECDH 라이브러리
- **컴파일러** : g++  
- **실행 환경** : VSCode 2022  
- **주요 특징**  
  - ECADD, ECDBL, Jacobian 좌표 연산  
  - 곱셈(OS, PS, Karatsuba), 제곱, 역원(FLT, EEA), 리덕션(Fast, Montgomery)  
  - ECC 스칼라 곱 (Left-to-Right, Right-to-Left)  
  - ECDH shared value 도출 및 외부 비교 매체 검증  

---

## 5. ⚡ 성능 측정
- **환경** : Laptop (동일 환경에서 동적 vs 정적 비교)  
- **측정 도구** : `__rdtsc`, 테스트 벡터 20,000개  
- **결과**  
  - 곱셈: OS vs PS → 큰 차이 없음  
  - 곱셈 vs 제곱 → 속도 차이 확인됨  
  - Fast Reduction → Montgomery 대비 유의미하게 빠름  
  - 역원(EEA vs FLT) → 큰 차이 없음  

---

## 6. 💡 활용 사례
- **Case 1** : 암호학과 학생의 학습용 라이브러리  
  - 슈도코드를 따라가며 구현된 동적 연산  
  - 다양한 정적 연산(ECC, Karatsuba, FLT, EEA 등)을 학습 가능  
- **Case 2** : ECDH 정적 라이브러리를 활용한 TLS 1.3 프로토콜 제작  

---

## 7. 💰 수익 모델
- **모델 1** : 암호학 학습용 교재 + 코드 패키지 → **25,000원**  
- **모델 2** :  
  - ECDSA 추가 구현 → AES-GCM과 결합 → **SJS-TLS 1.3 제작**  
  - GPU 포팅 → 대량 데이터 암호 프로토콜 → **200만원**  
  - GPU GCM 로직 특허 출원 진행 중  

---

## 8. ⚙️ 실행 방법

### 🔸 Linux (gcc, g++)
```bash
make
./run_dynamic
./run_static
🔸 Windows (Visual Studio)
솔루션 열기

빌드 후 실행

9. 📂 코드 구조 (예시)
csharp
코드 복사
/Airplane_300km
 ├── dynamic_lib/        # 동적 Big Number Library
 │   ├── add.c
 │   ├── mul.c
 │   ├── div.c
 │   └── ...
 ├── static_lib/         # 정적 ECDH Library
 │   ├── bignum.c
 │   ├── ecc_add.c
 │   ├── ecc_mul.c
 │   └── ...
 ├── tests/              # Python 검증툴
 │   └── verify.py
 ├── CMakeLists.txt
 └── README.md
✍️ Contributors : [위승주], [이재석]
🔒 Lab : Cryptography Optimization & Application Lab
