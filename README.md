# EvolutionTree

Evolution of a tree where the environment is customizable by the user. The fitness function is based on how much sunlight a tree is able to catch on its leaves.

You can find a link to download the [full executable here](https://mega.nz/#!JAQGUagA!qUTmYWQTm0dmkJJs1m_vgQYtUTOzTDhMj2VHq9V-Ht0)


![Evolved Tree](http://i.imgur.com/Hf8dNUa.jpg)

## Algorihm

This project was undertaked to learn how one could create a program using which uses the genetic algorithm to grow a tree that adapts to its environment in the best way. To learn the importance of mutation versus inheritance, and how to best represent the DNA.
⋅⋅*
## Goals

### Primary

1. Create a working instance of the genetic algorithm which represents a tree with a stem, a number of branches which attach to the tree, and a number of leafs that attach to branches.

2. Create a program with a user interface that allows the user to customize an environment for which to evolve a tree. These customizable options shall include:
  * Possibility to create obstructing actors at a chosen location.
  * Possibility to specify the location from which the sun rays originate, or use a fitness model where the rays come from several locations.
  * Possibility to specify the number of branches and leafs of the tree.

3. Data representation of current state of the simulation. This data shall include
 * Fitness of the current best tree.
 * Visual representation of the current best tree.
 * Fitness of the current average tree.

### Secondary

1. Possibility for customizable complexity of the tree, for example branches on other branches, differently sized branches, different stems etc.
2. High resolution / Better looking model representating the current best tree.
3. Make the mutations intelligent, making mutations that are likely to be beneficial more likely to appear.
4. Disallow overlapping branches/leafs/objects, making things not grow "into" each other.


### Tertiary

1. Possibility to export/import a population of trees to/from file.

## Breeding algorithm

There are two different breeding algorithms we will use and compare in the program, one sexual and one asexual, anectodal testing has this far seemed to indicate that sexual reproduction is much more effective even when the DNA crossing algorithm is uninformed.

Every generation a number of trees are killed of and replaced with a new generation of trees, which are based on the trees that didn't get killed of.

In our simulation logic class, we have a list of all the current trees, this list is then sorted based on each trees evaluated fitness. When the list is sorted, a culling is done to kill of a random number of trees, where the chance for a tree to be culled is linearly correlated with its position in the list. The actual code looks like this:

```
		// for all trees except the best, randomly decide if it dies or not, but where better trees (lower in the list) have a higher chance of survival, on average, a portion the size (1 / cullingConstant) will die and be replaced each generation..
		for (int32 i = 1; i < trees.Num(); ++i) {
			if (random.FRand() * trees.Num() * cullingConstant > i) {
				winners.Add(trees[i]);
			}
			else {
				losers.Add(trees[i]);
			}
		}
```

Where obviously the list of trees called "winners" are the trees that don't die and the list called "losers" are the ones that do.

After this code the losers are replaced depending on the current reproduction algorithm.


### Asexual reproduction

Our asexual reproduction works by duplicating a random tree in the winners list, and mutating it.

## Sexual reproduction

Our sexual reproduction works by randomly combining trees and leaves from two different trees and making a new one out of them, it keeps the properties of each branch and leaf intact, and does not for example combine the different rotations of two leafs. For now, the code looks like this:


```

vector<float> ATree::createChildDNA(ATree* otherParent) {

	vector<float> DNA;
	for (int i = 0; i < branches.Num(); ++i) {
		ATree* t;
		ABranch* b;
		if (random.FRand() < .5) {
			t = this;
			b = branches[i];
		}
		else {
			t = otherParent;
			b = otherParent->branches[i];
		}
		DNA.push_back(b->GetActorLocation().X - t->GetActorLocation().X);
		DNA.push_back(b->GetActorLocation().Y - t->GetActorLocation().Y);
		DNA.push_back(b->GetActorLocation().Z - t->GetActorLocation().Z);
		DNA.push_back(b->GetActorRotation().Pitch);
		DNA.push_back(b->GetActorRotation().Yaw);
		DNA.push_back(b->GetActorRotation().Roll);

	}
	for (int i = 0; i < leafs.Num(); ++i) {
		ALeaf* l = (random.FRand() < .5) ? leafs[i] : otherParent->leafs[i];
		DNA.push_back(l->attachedToIndex);
		DNA.push_back(l->branchOffset);
		DNA.push_back(l->offsetVector.X);
		DNA.push_back(l->offsetVector.Y);
		DNA.push_back(l->offsetVector.Z);
		DNA.push_back(l->GetActorRotation().Pitch);
		DNA.push_back(l->GetActorRotation().Yaw);
		DNA.push_back(l->GetActorRotation().Roll);

	}

	return DNA;
}

void ATree::buildFromDNA(vector<float> DNA) {
	int currPos = 0;
	for (int i = 0; i < branches.Num(); ++i) {
		branches[i]->SetActorLocation(FVector(DNA[currPos] + GetActorLocation().X, DNA[currPos + 1] + GetActorLocation().Y, DNA[currPos + 2] + GetActorLocation().Z));
		branches[i]->SetActorRotation(FRotator(DNA[currPos + 3], DNA[currPos + 4], DNA[currPos + 5]));
		currPos += 6;
	}
	for (int i = 0; i < leafs.Num(); ++i) {
		leafs[i]->attachedToIndex = DNA[currPos];
		leafs[i]->branchOffset = DNA[currPos + 1];
		leafs[i]->offsetVector = FVector(DNA[currPos + 2], DNA[currPos + 3], DNA[currPos + 4]);
		leafs[i]->SetActorRotation(FRotator(DNA[currPos + 5], DNA[currPos + 6], DNA[currPos + 7]));

		currPos += 8;

		leafs[i]->updateLocation(branches[leafs[i]->attachedToIndex]->getPositionOnBranch(leafs[i]->branchOffset));
	}

}
```
