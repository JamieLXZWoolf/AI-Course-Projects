from math import pow, sin, cos
from random import random, randint, sample
from matplotlib.pyplot import plot, show
from bisect import bisect_right
from itertools import accumulate
from statistics import mean
from numpy.random import choice
import numpy as np


class Genetic_Algorithm(object):
    def __init__(self, function, pop_size, upper_limit, chromosome_len, select_prob, mutation_prob, eliminate_threshold,
                 generations, t_s):
        self._function = function
        self._pop_size = pop_size
        self._upper_limit = upper_limit
        self._chromosome_len = chromosome_len
        self._select_prob = select_prob
        self._mutation_prob = mutation_prob
        self._eliminate_threshold = eliminate_threshold
        self._generations = generations
        self._t_s = t_s
        self._mean_per_generation = []
        self._max_per_generation = []
        self._indi_per_generation = []
        self._init_pop = [[]]
        for i in range(pop_size - 1):
            temp = []
            for j in range(chromosome_len):
                temp.append(randint(0, 1))
            self._init_pop.append(temp)

    def _binary_2_decimal(self, binary):
        """ Convert a binary chromosome into a floating-point decimal in the range of [0, chromosome_len]."""
        t = 0
        for i in range(len(binary)):
            t += binary[i] * (pow(2, i))
        return t * self._upper_limit / (pow(2, self._chromosome_len) - 1)

    def _get_best_and_average(self, pop, fitnesses):
        best_idx = fitnesses.index(max(fitnesses))
        return fitnesses[best_idx], self._binary_2_decimal(pop[best_idx]), mean(fitnesses)

    def _get_population_val(self, pop):
        """Evaluate all the results in the population for a given function(lambda expression)."""
        res = []
        for c in pop:
            res.append(self._function(self._binary_2_decimal(c)))
        return res

    def _eliminate_and_get_fitness(self, fitnesses):
        """Eliminate the ones less than a threshold."""
        res = []
        for v in fitnesses:
            if v < self._eliminate_threshold:
                res.append(self._eliminate_threshold)
            else:
                res.append(v)
        return res

    def _select_rws(self, pop, fitnesses, new_pop):
        total = sum(fitnesses)

        for i in range(self._pop_size - int((self._pop_size * self._select_prob / 2)) * 2):
            chosen = choice(np.arange(self._pop_size), p=np.array(fitnesses)/total)
            new_pop.append(pop[chosen])

    def _select_tournament(self, pop, fitnesses, new_pop):
        fitness = lambda i: fitnesses[i]
        complete = lambda competitors: max(competitors, key=fitness)

        for i in range(self._pop_size - int((self._pop_size * self._select_prob / 2)) * 2):
            chosen = complete(sample(range(self._pop_size), self._t_s))
            new_pop.append(pop[chosen])

    def _parents_crossover_tournament(self, pop, fitnesses, new_pop):
        fitness = lambda i: fitnesses[i]
        complete = lambda competitors: max(competitors, key=fitness)

        for i in range(int(self._pop_size * self._select_prob / 2)):
            competitors_1 = sample(range(self._pop_size), self._t_s)
            competitors_2 = sample(range(self._pop_size), self._t_s)
            father, mother = complete(competitors_1), complete(competitors_2)
            child_1, child_2 = self._crossover(pop[father], pop[mother])
            new_pop.extend([child_1, child_2])

    def _parents_crossover_rws(self, pop, fitnesses, new_pop):
        """Perform selection using roulette wheel selection method."""
        total = sum(fitnesses)
        wheel = list(accumulate([i/total for i in fitnesses]))

        for i in range(int(self._pop_size * self._select_prob / 2)):
            father_idx = bisect_right(wheel, random())
            father = pop[father_idx]
            mother_idx = (father_idx + 1) % len(wheel)
            mother = pop[mother_idx]
            child_1, child_2 = self._crossover(father, mother)
            new_pop.extend([child_1, child_2])

    def _crossover(self, father, mother):
        """Crossover the parents."""
        cross_point = randint(0, self._chromosome_len - 1)
        temp_1 = father[:cross_point]
        temp_2 = mother[:cross_point]
        temp_1.extend(mother[cross_point:])
        temp_2.extend(father[cross_point:])
        return temp_1, temp_2

    def _mutate(self, pop):
        """Mutation."""
        for i in range(self._pop_size):
            if random() < self._mutation_prob:
                mutation_point = randint(0, self._chromosome_len - 1)
                if mutation_point > len(pop[i]) - 1:
                    continue
                orig = pop[i][mutation_point]
                pop[i][mutation_point] = 0 if orig == 1 else 0

    def _evolve(self, mode):
        """Perform evolution."""
        pop = self._init_pop
        # print("Running genetic algorithm...")
        for i in range(self._generations + 1):
            fitnesses = self._get_population_val(pop)
            best_fitness, best_individual, mean = self._get_best_and_average(pop, fitnesses)
            print("Generation {}, best_individual: {}, best_fitness: {}, mean_fitness: {}".format(i, best_individual,
                                                                                                  best_fitness, mean))
            self._mean_per_generation.append(mean)
            self._indi_per_generation.append(best_individual)
            self._max_per_generation.append(best_fitness)
            if i == self._generations:
                break
            fitnesses = self._eliminate_and_get_fitness(fitnesses)
            new_pop = []

            if mode=="rws":
                self._select_rws(pop, fitnesses, new_pop)
                self._parents_crossover_rws(pop, fitnesses, new_pop)
            elif mode=="tournament":
                self._select_tournament(pop, fitnesses, new_pop)
                self._parents_crossover_tournament(pop, fitnesses, new_pop)
            if len(new_pop) != self._pop_size:
                print("wrong")
            pop = new_pop
            self._mutate(pop)

    def run(self, mode):
        """Show the analysis for the algorithm."""
        self._evolve(mode)
        best_gen_max = self._max_per_generation.index(max(self._max_per_generation))
        print("Best individual among all generations is from generation {}, it is {}, "
              "fitness_value is {}".format(best_gen_max, self._indi_per_generation[best_gen_max],
                                           self._max_per_generation[best_gen_max]))
        X = []
        Y = []
        for i in range(self._generations):
            X.append(i+1)
            Y.append(self._mean_per_generation[i])
        plot(X, Y)
        show()

    def test(self, mode):
        self._evolve(mode)
        best_gen_max = self._max_per_generation.index(max(self._max_per_generation))
        return self._max_per_generation[best_gen_max]

if __name__ == '__main__':
    ga = Genetic_Algorithm(lambda x: x + 10 * sin(5 * x) + 7 * cos(4 * x), 200, 9, 17, 0.4, 0.01, 0, 200, 5)
    ga.run("tournament")
