# EvolutionTree

Evolution of a tree where the environment is customizable by the user. The fitness function is based on how much sunlight a tree is able to catch on its leaves.

## Algorihm

This project was undertaked to learn how one could create a program using which uses the genetic algorithm to grow a tree that adapts to its environment in the best way. To learn the importance of mutation versus inheritance, and how to best represent the DNA.

## Goals

### Primary

1. Create a working instance of the genetic algorithm which represents a tree with a stem, a number of branches which attach to the tree, and a number of leafs that attach to branches.

2. Create a program with a user interface that allows the user to customize an environment for which to evolve a tree. These customizable options shall include:
..* Possibility to create obstructing actors at a chosen location.
..* Possibility to specify the location from which the sun rays originate, or use a fitness model where the rays come from several locations.
..* Possibility to specify the number of branches and leafs of the tree.

3. Data representation of current state of the simulation. This data shall include
..* Fitness of the current best tree.
..* Visual representation of the current best tree.
..* Fitness of the current average tree.

### Secondary

1. Possibility for customizable complexity of the tree, for example branches on other branches, differently sized branches, different stems etc.
2. High resolution / Better looking model representating the current best tree.

### Tertiary

1. Possibility to export/import a population of trees.
