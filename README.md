# Graphics
CSE4170 Intro to Computer Graphics | OpenGL Projects

## :tulip: Project 3: 2D Transformation Animation
* OpenGL 이차원 모델링 변환
* 모자 안에 물건들이 들어가면서 색이 바뀌는 요술모자 애니메이션, 멀리서부터 앞으로 가까이 오는듯한 자동차 움직임 생성   
<img src="https://user-images.githubusercontent.com/37509450/93300941-ad6ee280-f832-11ea-9808-ea7779b81c6e.png" height="150px" ></img>
<img src="https://user-images.githubusercontent.com/37509450/93300990-bd86c200-f832-11ea-926d-bfa8c257ddd1.png" height="150px" ></img>
<img src="https://user-images.githubusercontent.com/37509450/93301605-a85e6300-f833-11ea-95ec-98fd844c1dd9.png" height="150px" ></img>
<img src="https://user-images.githubusercontent.com/37509450/93301812-f4a9a300-f833-11ea-91cf-4f450659cdee.png" height="150px" ></img>
<img src="https://user-images.githubusercontent.com/37509450/93300724-4cdfa580-f832-11ea-9b4b-a589642799a4.png" height="150px" ></img>
<img src="https://user-images.githubusercontent.com/37509450/93300860-8dd7ba00-f832-11ea-909f-c4f3d02fe575.png" height="150px" ></img>
* 2차원 동적 효과 구현
  * 케이크 : fabs함수, 0~360도 회전
  * 비행기 : 음의 2차함수, 동선을 따라 방향 변화
  * 집 : 흩뿌리는듯한 효과
  * 자동차 : 여러 크기의 y축 sin함수(sin(2x), 2sin(2x), 3sin(2x) 등)를 합친 동선, scale 변화
  * 칵테일 : 90도 회전
  * 나무 : 반원 둘레를 따라 회전
* 물체 생성
  * 나무, 도로, 사과
  
  
## :tulip: Project 4: Simple 3D Transformation
* OpenGL 삼차원 모델링 변환   
<img src="https://user-images.githubusercontent.com/37509450/93302697-5d454f80-f835-11ea-9052-f653d8c3ad58.png" height="150px"></img>
<img src="https://user-images.githubusercontent.com/37509450/93302918-a4cbdb80-f835-11ea-89d3-6f4a517d1f2d.png" height="150px"></img>


## :tulip: Project 5: Final 3D Transformation
* OpenGL 삼차원 모델링 변환 기말 프로젝트
* 물체 동적 움직임 생성
  * 스파이더 : 나선형을 그리며 위로 올라가는 움직임
  * 사람 : 반원 모양의 언덕 두 개를 좌우를 왔다갔다하며 오르고 내리며 등산하는 듯한 움직임
  * 호랑이 : 정사각형 둘레를 따라서 방향을 바꿔가며 돌면서 오르락내리락하는 움직임
  * 아이언맨 : 360도로 회전하면서 원을 둘레로 회전하며 날아가는 듯한 움직임   
  <img src="https://user-images.githubusercontent.com/37509450/93304511-173dbb00-f838-11ea-9174-d4de62943328.png" height="250px"></img>
* 광원 설정
  * 0번 점 광원 : 세상좌표계 기준, 위에서 아래 floor로 향함
  * 1번 노란색 스폿 광원
  * 2번 카메라 고정 파란색 스폿 광원 : w 타이핑 시 blind 효과
  * 3번 아이언맨의 발에 고정된 빨간색 스폿 광원   
 <img src="https://user-images.githubusercontent.com/37509450/93304204-97175580-f837-11ea-822b-c6a83668114f.png" height="250px"></img>
 <img src="https://user-images.githubusercontent.com/37509450/93305093-03468900-f839-11ea-986a-a896f72890cb.png" height="250px"></img>
  
* 카메라 움직임 설정
  * 카메라 프레임의 u, v, n축 방향으로 이동(translation) : 마우스 드래그로 조작
  * 카메라 프레임의 u, v, n축 둘레로 회전(rotation) : 키보드 키 u/v/n 클릭 후 좌우 방향키로 조작
  * Field of View 인자를 변경하여 줌인/줌아웃 효과 : 키보드 상하방향키로 조작   
  <img src="https://user-images.githubusercontent.com/37509450/93304669-5a982980-f838-11ea-999c-8af8f4c5506a.png" height="250px"></img>
  <img src="https://user-images.githubusercontent.com/37509450/93304773-83202380-f838-11ea-9448-c4cb827097ad.png" height="250px"></img>
  
* 텍스쳐 적용
  * 드래곤

* 스크린 효과
  * 하트모양 스크린 효과 : e 키로 조작   
  <img src="https://user-images.githubusercontent.com/37509450/93305013-e14d0680-f838-11ea-95fe-bdb83bd105f0.png" height="250px"></img>
  
* 운전석 카메라 프레임 효과
  * r 키 클릭 시 아이언맨을 뒤에서 바라보는 카메라 구도로 바뀜.      
  <img src="https://user-images.githubusercontent.com/37509450/93305068-f9248a80-f838-11ea-9edc-c6b935f50747.png" height="250px"></img>


  
