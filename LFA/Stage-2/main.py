# Mateescu Viorel-Cristian, 332CBb
import sys
from contextlib import redirect_stdout
from typing import Dict, List


# State in the DFA - I have used a class as suggested on ppcarte.
class State:
    def __init__(self, state_idx):
        self.stateIdx = state_idx

    # As suggested in the DFA laboratory on ppcarte I'm defining a hash-function in order to use dictionaries over
    # states consisting of the hashing of the state's index.
    def __hash__(self):
        return hash(self.stateIdx)

    # In order to compare two states by their indices I have overridden the eq method.
    def __eq__(self, other):
        return self.stateIdx == State(other).stateIdx

    # For easier printing I have also overridden the str method to return the State's index.
    def __str__(self):
        return str(self.stateIdx)

    # Compare two States by their indexes.
    def __lt__(self, other):
        return self.stateIdx < other.stateIdx


# Configuration - tuple of State and string.
Conf = (State, str)


# The DFA.
class DFA:
    def __init__(self):
        self.alphabet: set[str] = set()
        self.initialState: State = None
        self.delta: Dict[int, Dict[str, State]] = dict()
        self.states: set[State] = set()
        self.finalStates: set[State] = set()
        self.sinkStates: set[State] = set()

    # Function for printing a DFA.
    def print_dfa(self):
        # Print the Alphabet
        for char in self.alphabet:
            print(char, end='')
        print()

        # Print the Number of States.
        print(len(self.states))

        # Print the Initial State.
        print(self.initialState)

        # Print the Final States
        for state in self.finalStates:
            print(state, end=' ')
        print()

        # Print the DFA's Transitions.
        for state in self.states:
            for key in self.delta[hash(state)].keys():
                print(str(state) + ",'" + key + "'," + str(self.delta[hash(state)][key]))


# The NFA.
class NFA:
    def __init__(self):
        self.alphabet: set[str] = set()
        self.initialState: State = None
        self.delta: Dict[int, Dict[str, List[State]]] = dict()
        self.finalStates: set[State] = set()
        self.states: set[State] = set()

    # Function for printing an NFA.
    def print_nfa(self):
        # Print the Alphabet.
        for char in sorted(self.alphabet):
            print(char, end='')
        print()

        # Print the number of States.
        print(len(self.states))

        # Print the Initial State.
        print(self.initialState)

        # Print the Final States.
        for state in self.finalStates:
            print(state, end=' ')
        print()

        # Print the NFA's Transitions.
        for state in self.states:
            if hash(state) in self.delta:
                for key in self.delta[hash(state)].keys():
                    for next_state in self.delta[hash(state)][key]:
                        print(str(state) + ",'" + key + "'," + str(next_state))

    # Function for computing an NFA's Epsilon Closures.
    def get_epsilon_closures(self):
        # Store the closures as a dictionary(State, List[State])
        epsilon_closures = dict()

        # Compute each state's closure.
        for state in self.states:
            # State's to visit.
            to_visit = [state]

            # The Closure.
            closure = [state]

            # Already visited States.
            visited = set()

            # While there are still states that should be visited:
            while len(to_visit) > 0:
                next_state = to_visit.pop()

                # If it has already been visited move to the next one.
                if next_state in visited:
                    continue

                visited.add(next_state)

                if hash(next_state) in self.delta:
                    # Check if the state has epsilon transitions.
                    if "ε" in self.delta[hash(next_state)].keys():
                        # Add the destination states to the closure and to the list of states to visit.
                        closure += list(nfa.delta[hash(next_state)]["ε"])
                        to_visit += list(self.delta[hash(next_state)]["ε"])

            # Add the closure to the closures dictionary.
            epsilon_closures[hash(state)] = (sorted(set(closure)))

        return epsilon_closures


# Regex Base Class.
class Expr:
    pass


# The Symbol.
class Symbol(Expr):
    _char: str

    def __init__(self, c: str):
        self.char = c

    def __str__(self):
        return self.char


# The Kleene Star Operator - Not used in the Current Stage.
class Star(Expr):
    _expr: Expr

    def __init__(self, expr: Expr):
        self.expr = expr

    def __str__(self):
        return "(" + str(self.expr) + ")*" if isinstance(self.expr, Concat) or isinstance(self.expr, Union) \
            else str(self.expr) + "*"


# The Concat Operator - Not used in the Current Stage.
class Concat(Expr):
    _expr1: Expr
    _expr2: Expr

    def __init__(self, expr1: Expr, expr2: Expr):
        self._expr1 = expr1
        self._expr2 = expr2

    def __str__(self):
        parsed1 = "(" + str(self._expr1) + ")" if isinstance(self._expr1, Union) else str(self._expr1)
        parsed2 = "(" + str(self._expr2) + ")" if isinstance(self._expr2, Union) else str(self._expr2)

        return parsed1 + parsed2


# The Union Operator - Not used in the Current Stage.
class Union(Expr):
    _expr1: Expr
    _expr2: Expr

    def __init__(self, expr1: Expr, expr2: Expr):
        self._expr1 = expr1
        self._expr2 = expr2

    def __str__(self):
        return str(self._expr1) + " U " + str(self._expr2)


# Function used for reducing the Stack in order to generate the equivalent NFA for the Regex given as input.
def reduce_stack(stack: list, index: List[int]):
    # Check if there are any elements in the stack.
    while len(stack) > 1:
        curr_elem = stack[-1]

        # If the top of the stack is an NFA check if the stack can be reduced.
        if isinstance(curr_elem, NFA):
            prev_elem = stack[-2]

            # Reduce the Stack by building the equivalent STAR NFA.
            if prev_elem == "STAR":
                stack.pop()
                stack.pop()
                stack.append(star_nfa(curr_elem, index))
            # Reduce the Stack by building the equivalent PLUS NFA.
            elif prev_elem == "PLUS":
                stack.pop()
                stack.pop()
                stack.append(plus_nfa(curr_elem, index))
            # Reduce the Stack by building the equivalent CONCAT/UNION NFA.
            elif isinstance(prev_elem, NFA):
                stack.pop()
                stack.pop()
                op = stack.pop()

                # Check if the operator is CONCAT or UNION and build the equivalent NFA.
                if op == "CONCAT":
                    stack.append(concat_nfa(prev_elem, curr_elem))
                elif op == "UNION":
                    stack.append(union_nfa(prev_elem, curr_elem, index))
            else:
                break
        else:
            break

    # Return the reduced stack.
    return stack


# Function used for converting a Regex to an NFA - I started my implementation based on the one from the Regex
# Laboratory, but instead of pushing Mini Regexes to the Stack, I push mini NFAs.
def regex_to_nfa(s: str, index: [int]):
    tokens = s.split(' ')
    stack = []

    for token in tokens:
        # If the token is a Symbol push the corresponding Symbol NFA to the stack, else it is an operator and should
        # be pushed directly to the stack.
        if token != "CONCAT" and token != "UNION" and token != "STAR" and token != "PLUS":
            stack.append(symbol_nfa(Symbol(token), index))
        else:
            stack.append(token)

        # Reduce the stack.
        stack = reduce_stack(stack, index)

    # Return the corresponding NFA for the Regex.
    return stack.pop()


# Function for creating a Symbol NFA, implemented using the algorithm from the lecture.
# Base case: c E alphabet, initial state --c--> final state
def symbol_nfa(symbol: Symbol, index: List[int]):
    nfa = NFA()

    # Add the character to the alphabet.
    nfa.alphabet.add(symbol.char)

    # Create the initial and final states.
    initial_state = State(index[0])
    final_state = State(index[0] + 1)
    index[0] += 2

    nfa.states.add(initial_state)
    nfa.states.add(final_state)

    nfa.initialState = initial_state
    nfa.finalStates = {final_state}

    # Add the transition from the initial state to the final state via the character.
    nfa.delta[hash(initial_state)] = dict()
    nfa.delta[hash(initial_state)][symbol.char] = [final_state]

    return nfa


# Function for creating a STAR NFA, implemented using the algorithm from the lecture.
#                              _____________e______________
#                              |                          |
#                              v                          |
# new initial state --e--> NFA's initial state -...-> NFA's final state --e--> new final state
#        |                                                                          ^
#        |                                                                          |
#        ----------------------------------e-----------------------------------------
def star_nfa(nfa: NFA, index: List[int]):
    # Crete the new initial and final states.
    initial_state = State(index[0])
    final_state = State(index[0] + 1)
    index[0] += 2

    nfa_final_state, *_ = nfa.finalStates

    # Add the Epsilon transitions necessary for the Star NFA.
    nfa.delta[hash(initial_state)] = dict()
    nfa.delta[hash(initial_state)]["ε"] = [nfa.initialState]

    nfa.delta[hash(initial_state)]["ε"].append(final_state)

    nfa.delta[hash(nfa_final_state)] = dict()
    nfa.delta[hash(nfa_final_state)]["ε"] = [nfa.initialState]

    nfa.delta[hash(nfa_final_state)]["ε"].append(final_state)

    nfa.initialState = initial_state
    nfa.finalStates = {final_state}

    nfa.states.add(initial_state)
    nfa.states.add(final_state)

    return nfa


# Function for creating a PLUS NFA, implemented by creating a copy of the initial NFA. After that I create a STAR NFA
# from the copy of the initial NFA and concatenate it with the initial NFA.
def plus_nfa(nfa: NFA, index: List[int]):
    nfa1 = NFA()

    # Create a copy of the NFA and keep a state Map for renaming the States in te NFA's copy.
    state_map: Dict[int, State] = dict()

    # Clone the NFA and change the states' indexes.
    for state in nfa.states:
        if hash(state) not in state_map:
            state_map[hash(state)] = State(index[0])
            index[0] += 1

        new_state = state_map[hash(state)]
        nfa1.delta[hash(new_state)] = dict()
        nfa1.states.add(new_state)

        if hash(state) in nfa.delta:
            for key in nfa.delta[hash(state)].keys():
                nfa1.delta[hash(new_state)][key] = []
                for next_state in nfa.delta[hash(state)][key]:
                    if hash(next_state) not in state_map:
                        state_map[hash(next_state)] = State(index[0])
                        index[0] += 1

                    corresponding_next_state = state_map[hash(next_state)]
                    nfa1.delta[hash(new_state)][key].append(corresponding_next_state)

    nfa1.initialState = state_map[hash(nfa.initialState)]

    for final_state in nfa.finalStates:
        nfa1.finalStates.add(state_map[hash(final_state)])

    nfa1.alphabet = nfa.alphabet.copy()

    # Create a star NFA from the clone.
    nfa2 = star_nfa(nfa1, index)

    # Concatenate the initial NFA with the STAR NFA in order to build the PLUS NFA.
    return concat_nfa(nfa, nfa2)


# Function for creating an UNION NFA, implemented using the algorithm from the lecture.
# new initial state --e--> NFA1's initial state -...-> NFA1's final state --e--> new final state
#        |                                                                             ^
#        |                                                                             |
#        ------e-----> NFA2's initial state -...-> NFA2's final state --------e---------
def union_nfa(nfa1: NFA, nfa2: NFA, index: List[int]):
    # Create the new initial and final states.
    initial_state = State(index[0])
    final_state = State(index[0] + 1)
    index[0] += 2

    nfa1_final_state, *_ = nfa1.finalStates
    nfa2_final_state, *_ = nfa2.finalStates

    # Add the necessary Epsilon transitions.
    nfa1.delta[hash(initial_state)] = dict()
    nfa1.delta[hash(initial_state)]["ε"] = [nfa1.initialState]
    nfa1.delta[hash(initial_state)]["ε"].append(nfa2.initialState)

    nfa1.delta[hash(nfa1_final_state)] = dict()
    nfa1.delta[hash(nfa1_final_state)]["ε"] = [final_state]

    nfa2.delta[hash(nfa2_final_state)] = dict()
    nfa2.delta[hash(nfa2_final_state)]["ε"] = [final_state]

    nfa1.initialState = initial_state
    nfa1.finalStates = {final_state}

    nfa1.states.add(initial_state)
    nfa1.states.add(final_state)

    # Combine the two NFAs.
    nfa1 = combine_nfas(nfa1, nfa2)

    return nfa1


# Function for creating an UNION NFA, implemented using the algorithm from the lecture.
# NFA1's initial state -...-> NFA1's final state --e--> NFA2's initial state -...-> NFA2's final state
def concat_nfa(nfa1: NFA, nfa2: NFA):
    nfa1_final_state, *_ = nfa1.finalStates

    # Add the necessary Epsilon transition.
    nfa1.delta[hash(nfa1_final_state)] = dict()
    nfa1.delta[hash(nfa1_final_state)]["ε"] = [nfa2.initialState]

    # Combine the two NFAs.
    nfa1 = combine_nfas(nfa1, nfa2)

    # Update the NFA's new Final State.
    nfa1.finalStates = nfa2.finalStates.copy()

    return nfa1


# Function for combining two NFA's.
def combine_nfas(nfa1: NFA, nfa2: NFA):
    # Join the states sets.
    nfa1.states.update(nfa2.states)

    # Join the delta functions.
    nfa1.delta.update(nfa2.delta)

    # Join the alphabets sets.
    nfa1.alphabet.update(nfa2.alphabet)
    return nfa1


# Function for creating a DFA based on input generated from an NFA.
def create_dfa(delta: Dict[int, Dict[str, State]], states, alphabet, final_states):
    dfa = DFA()

    # Set the alphabet, the initial state and the final states.
    dfa.alphabet = sorted(alphabet)
    dfa.initialState = State(0)
    dfa.finalStates = final_states

    # Create a possible Sink State.
    sink_state = State(len(states))
    is_sink_necessary = False

    # Check if transitions to the Sink State should be added for each State in the DFA.
    for i in range(len(states)):
        state = State(i)
        dfa.states.add(state)

        # The State has no transitions in the delta function, transitions to the Sink State via any character in the
        # alphabet should be added.
        if hash(state) not in delta:
            delta[hash(state)] = dict()
            for key in alphabet:
                delta[hash(state)][key] = sink_state

            is_sink_necessary = True
        # Check if there are any characters in the alphabet that don't have a transition from the current state via it
        # in the delta function and add them with the Sink State as the destination State.
        else:
            for key in alphabet:
                if key not in delta[hash(state)].keys():
                    delta[hash(state)][key] = sink_state
                    is_sink_necessary = True

    # Add the Sink State transitions to self on any character in the alphabet if necessary.
    if is_sink_necessary:
        delta[hash(sink_state)] = dict()
        for key in alphabet:
            delta[hash(sink_state)][key] = sink_state

        dfa.states.add(sink_state)
        dfa.sinkStates.add(sink_state)

    # Finally set the delta function.
    dfa.delta = delta

    return dfa


# Function for converting an NFA to a DFA, implementation based on the algorithm from the lecture.
def nfa_to_dfa(nfa: NFA):
    # Compute the NFA's Epsilon Transitions.
    epsilon_closures = nfa.get_epsilon_closures()

    closures = [epsilon_closures[hash(nfa.initialState)]]
    visited_closures: List[List[State]] = []
    state_index = 0

    # DFA's components.
    delta: Dict[int, Dict[str, State]] = dict()
    final_states: set[State] = set()
    alphabet = nfa.alphabet.copy()

    # Compute the DFA's transitions.
    while len(closures) > 0:
        transitions: Dict[str, List[State]] = {}

        # Check if the current closure has been reached before and get its corresponding state in the DFA.
        closure = closures.pop()
        if closure in visited_closures:
            new_state = State(visited_closures.index(closure))
        else:
            new_state = State(state_index)
            delta[hash(new_state)] = dict()
            visited_closures.append(closure)
            state_index += 1

        # Check if the corresponding state should be final or not.
        for state in closure:
            if state in nfa.finalStates:
                final_states.add(new_state)
                break

        # Save all the transitions from states in the closure on characters != "ε" in the transitions dictionary.
        for state in closure:
            if hash(state) in nfa.delta:
                for key in nfa.delta[hash(state)].keys():
                    if key != "ε":
                        for next_state in nfa.delta[hash(state)][key]:
                            if key not in transitions:
                                transitions[key] = []

                            transitions[key].append(next_state)

        # Compute the next closures that can be reached from the current closure.
        for key in transitions.keys():
            new_closure = []

            # Add to the closure all the states that can be reached from the current closure via the current key.
            for state in transitions[key]:
                new_closure.append(epsilon_closures[hash(state)])

            # Remove duplicates and check if the new closure has been reached before.
            new_closure = list(set(sum(new_closure, [])))
            if new_closure not in visited_closures:
                visited_closures.append(new_closure)
                closures.append(new_closure)

            # Add the transition from the state corresponding to the initial closure to the one corresponding to the
            # new closure.
            if hash(new_state) not in delta:
                delta[hash(new_state)] = dict()
            delta[hash(new_state)][key] = State(visited_closures.index(new_closure))

    # Create the equivalent DFA.
    return create_dfa(delta, visited_closures, alphabet, final_states)


if __name__ == '__main__':
    with open(sys.argv[1], "r") as input:
        # Read the regex.
        regex = input.readline()

        # Convert the Regex to an NFA.
        nfa = regex_to_nfa(regex, [0])

        # Convert the NFA to a DFA.
        dfa = nfa_to_dfa(nfa)

    # Print the DFA.
    with open(sys.argv[2], "w") as output:
        with redirect_stdout(output):
            dfa.print_dfa()
