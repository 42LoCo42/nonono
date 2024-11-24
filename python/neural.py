#!/usr/bin/env python3
import numpy as np

inputs  = np.random.rand(100, 2) * 10 - 5

def solver(point):
    return np.sign(np.prod(point))

outputs = map(solver, inputs)

def activation(val):
    return 1 / (1 + np.exp(-val))

def activationDeriv(val):
    ev = np.exp(val)
    return ev / (1 + ev) ** 2

def cost(val, real):
    return (val - real) ** 2

def costDeriv(val, real):
    return 2 * (val - real)

class Network:
    inputCount = 0
    layers = None

    # *args are neuron counts per layer
    def __init__(self, inputCount, *args):
        self.inputCount = inputCount
        counts = [inputCount, *args]
        # create random net
        self.layers = list(map(lambda left, right:
            (
                np.random.rand(left, right) * 2 - 1, # weights
                np.random.rand(right) * 2 - 1        # biases
            ), counts, counts[1:]
        ))

    def checkValidInputs(self, inputs):
        if len(inputs) != self.inputCount:
            raise ValueError(f"{len(inputs)} inputs but {self.inputCount} expected!")

    def run(self, inputs):
        self.checkValidInputs(inputs)

        current = inputs
        for (weights, biases) in self.layers:
            current = activation(np.matmul(current, weights) + biases)

        return current

    # TODO
    def learn(self, inputs, outputs):
        self.checkValidInputs(inputs)

        if len(inputs) != len(outputs):
            raise ValueError(f"{len(inputs)} Inputs and {len(outputs)} outputs - unequal sizes!")
