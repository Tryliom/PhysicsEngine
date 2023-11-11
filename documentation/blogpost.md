# Physics engine

It's a physics engine created during the [SAE Games Programming](https://www.sae.edu/ch-ge/games-programming-diploma-bachelor) formation, by [Alexis Haldy aka Tryliom](https://github.com/Tryliom).<br>

What's the meaning of this engine ?
==============================================================

Goal : Create a physics engine from scratch in C++.<br>
  * Understand Physics basics
  * Track our code
  * Understand how to optimize it

Demo scenes
==============================================================
## Planet Scene

Planets orbiting around a sun, the small circles are the planets and the center one (in yellow) is the sun.

We can see the gravitational force applied to the planets, the closer they are to the sun, the faster they go.
We can spawn planets and suns by clicking on the screen with different mass.

![](images/planet.gif)

## Trigger Scene

Circles, rectangles and polygons will change color depending on if they overlap, enter or exit a trigger.

- Yellow mean overlap
- Green outside border mean enter
- Red outside border mean exit

You can see the quadtree in the background (square borders), it's used to optimize the collision detection.

![](images/triggers.gif)

## Collision Scene

Circles and rectangles collide with each other and adapt their velocity, position.
At each collision, the shapes take on the same new color and change their inside color according to the state (enter and exit).

- Green inside mean enter
- Red inside mean exit

![](images/collisions.gif)

## Gravity Scene

You can spawn circles and rectangles, they are attracted by a force that pulls them downwards at each frame.<br>
You can edit the gravity force and the bounce factor of future spawned shapes.<br>
There is a static ground that can't be moved and that will collide with the shapes.

![](images/gravity.gif)

Quadtree
==============================================================

## What is a quadtree ?

A quadtree is a tree data structure in which each internal node has exactly four children. Quadtrees are the two-dimensional analog of octrees and are most often used to partition a two-dimensional space by recursively subdividing it into four quadrants or regions. The data associated with a leaf cell varies by application, but the leaf cell represents a "unit of interesting spatial information".<br>
This means that the quadtree will divide the screen into 4 parts, and each of these parts will be divided into 4 parts, and so on up to a certain depth defined by the user.<br>
In the engine, the quadtree is used to optimize the collision detection, it will only check the shapes that are in the same quadtree node (and children) as the shape that we want to check.<br>

In the engine, the max depth is 5 and max number of shape per node is 8.<br>

************************************
*.---------------.----------------.*
*|               |                |*
*|     Node 1    |    Node 2      |*
*|               |                |*
*|               |                |*
*.---------------.----------------.*
*|               |                |*
*|     Node 3    |    Node 4      |*
*|               |                |*
*|               |                |*
*'---------------'----------------'*
************************************

The Node 0 is the root node, it's the first node that will be created, it will be divided into 4 nodes (Node 1, Node 2, Node 3, Node 4) if there is more than 8 shapes (max shape per nodes) in the root node.<br>

## Advantages

Reduce the number of collision checks, the time complexity go through O(n) -> Olog(n) with this method as we divided the space in four.

## How I made it ?

I store all children in a simple vector, so I don't have to deal with pointers.<br>
The children index of a node index can be found by multiplying the node index by 4 and adding 1, 2, 3 or 4.<br>

### Insertion

First, the world update the bodies position and velocity.<br>
Then, the world insert all colliders in the quadtree.

The quadtree check if the collider overlap the node, if it's the case, the collider is inserted in the node.<br>
If the node is full, it will be divided into 4 nodes and all colliders will be inserted in the new nodes.<br>
If the collider overlap multiple children, it will be inserted in the parent node.<br>
The max number limit will be ignored if the node is a leaf node (it doesn't have children).<br>

### Calculate all possible pairs (simplified)

After inserting all colliders in the quadtree, the world will ask the quadtree to calculate all possible pairs.<br>
It will take all colliders in the root node and check if they overlap with simplified bound (rectangle), if it's the case, it will add them to the possible pairs list.<br>
Then, it will take all colliders in the children nodes and check if they overlap, if it's the case, it will add them to the possible pairs list.<br>
It will do this recursively until it reach the leaf nodes.<br>

Allocators
==============================================================

## What is an allocator ?

An allocator is a class that will allocate memory for you.<br>
It's used to avoid memory fragmentation and to improve the performance.<br>
The allocator used in the engine is a heap allocator, it's a simple allocator that will allocate memory on the heap.<br>
This is mainly used to monitor the memory usage and to avoid memory fragmentation.<br>

I made a heap, linear and free list allocator.<br>

Memory and performance tracking
==============================================================

Memory and performance tracking is done with the [Tracy profiler](https://github.com/wolfpld/tracy/tree/master) library.<br>
Here is a frame with the Trigger Scene with 500 circles and 500 rectangles.<br>

![](images/tracyFrame.png)

You can see that the engine is using 2MB of memory and that the CPU usage is around 12% (on a 3.8GHz CPU).<br>
The engine is running at 165FPS (vsync), the frame time is around 9ms and the physics time take around 80% of the total time.<br>

## Memory tracking

There is small allocations when inserting colliders each times a node size increase. The nodes keep the same size after that so there is less future allocations.<br>

![](images/insertColliders.png)

There is a big allocation when all possible pairs are put on the new collider pairs list (with verified collision).<br>
This is deallocated at the end of the collision detection.<br>

![](images/10secProfile.png)

The allocations grow with the time since nodes are not deallocated, the engine is using 2MB of memory after 10 seconds.<br>
All memory is deallocated when the engine is closed.<br>

Performance tracking over the development of the engine
==============================================================

I will start by showing how the engine was performing using Tracy at the beginning of the project, without any optimization nor quadtree.<br>
And continue with all the optimization that I made and how it improved the performance.<br>
It will be a frame with the Collision Scene with 500 circles and 500 rectangles every time. I will take 10sec of sample each time.<br>

## Without quadtree

At the beginning, I don't implement the quadtree, so the engine was checking all possible pairs at each frame.<br>
I also don't implement the memory tracking yet.<br>

![A frame](images/history1.png)
![The stats of the updateColliders](images/history1Stats.png)

We can see that a frame take around 30ms to be processed, the physics time take around 80% of the total time.<br>
At this time, I had a bug with the double rendering, so the engine was rendering twice the same frame, but it's not important for the performance.<br>

## With basic quadtree

I implemented the quadtree, but I didn't optimize it yet, and it has some problems.<br>
The collider were inserted in each node that they overlap, so a collider could be inserted multiple times in the quadtree.<br>
I use an unordered_set to store the colliders in the quadtree.<br>

![A frame](images/history2.png)
![The stats of the updateColliders](images/history2Stats.png)

### Difference with the previous frame

Frame time: 30ms -> 18ms<br>
Physics time: 80% -> 50%<br>

![Student t-test](images/ttest1-2.png)

## With optimized quadtree

- I optimized the quadtree, so a collider can only be inserted once in the quadtree.
- I still get all colliders using recursive calls and insert with the same vector.
- I still use an unordered_set to store the colliders in the quadtree.
- It uses now a FreeList allocator to allocate the colliders inside the quadtree, it created a small memory leak.
- I implemented the memory tracking.
- I fixed the double rendering bug.

![A frame](images/history3.png)
![The stats of the updateColliders](images/history3Stats.png)

We can see a lot of allocations, it's the free list allocator that allocate memory for the colliders.<br>

### Difference with the previous frame

Frame time: 18ms -> 8ms<br>
Physics time: 50% -> 70%<br>

![Student t-test](images/ttest2-3.png)

## Now, with the optimized quadtree and the optimized collision detection

- I removed the use of unordered_set, I use now a vector to store the colliders in the quadtree with a method to only insert a collider pair once.<br>
- I optimized the collision detection, I don't check anymore if the colliders are in the same quadtree node, I check if they overlap with simplified bound (rectangle).<br>
- I use a heap allocator to allocate the colliders inside the quadtree, it doesn't create memory leak anymore, but it's slower.<br>
- It's stable now, there is no more allocations.<br>

![A frame](images/history4.png)
![The stats of the updateColliders](images/history4Stats.png)

### Difference with the previous frame

Frame time: 8ms -> 9ms<br>
Physics time: 70% -> 73%<br>

A frame take more time to be processed, but the getColliderPairs function take less time.<br>

![Student t-test](images/ttest3-4.png)