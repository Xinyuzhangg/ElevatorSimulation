# Design Doc of Elevator Simulation 

## 1 User Interface

1. Control Units
   * Passengers number: P(Fixed to adjustable)[Init = 45]

   * Number of floors: L(Fixed to adjustable)[Init = 15]

   * Number of elevators: N(Fixed to adjustable)[Init = 3]

   * Load of elevator: K(10~20)[Init = 15]

   * Elevator speed: S(1~5)[Init = 3]

   * Transition time: T(2~10)[Init = 5]
2. Buttons

   * Start button

   * Stop button

3. Graphics Interface
   * Elevators

   * Building floors

## 2 Program Structure

	### 2.1 Macros

```c++
#define
```



### 2.2 Objects



1. Elevator

   ```c++
   enum Status = {Stop, Up, Down}
   class Elevator {
       private:
       	Status CurStatus;
       	int CurFloor;
       	int Capacity;
       	int CurLoad;
       public:
       
       	
               
              
   };
   ```

2. Person

   ```c++
   class Person {
       private:
       	int EndFloor;
       	int StartFloor;
       	int TranTime;
       public:
       	
       	
       	
       	
   
   };
   ```

3. Building

   ```c++
   class Building {
       private:
       	int Num_Person; 
       	int Num_Elevator;
       	int Num_Floor;
       public:
       	void Execute();
       	Building();
       public:
    	void Init();
       	void Render();
       	
       	
       
               
   
   };
   ```
   

## Program UML Design

![image-20200903164858943](C:\Users\Charles\AppData\Roaming\Typora\typora-user-images\image-20200903164858943.png)



## Task List

- [ ] Function design
- [ ] 

## Reference

<http://courses.cs.vt.edu/~cs1044/summerII06/projects/p4/project4.pdf>



