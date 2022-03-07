# Mateescu Viorel-Cristian, 332CBb
from typing import Dict, List

# State in the DFA - I have used a class as suggested on ppcarte.
class State:
    def __init__(self, state_idx):
        self.stateIdx = state_idx

    """
    As suggested in the DFA laboratory on ppcarte I'm defining a 
    hash-function in order to use dictionaries over states consisting 
    of the hashing of the state's index.
    """
    def __hash__(self):
        return hash(self.stateIdx)

    """
    In order to compare two states by their indices I have overridden the eq 
    method.
    """
    def __eq__(self, other):
        return self.stateIdx == State(other).stateIdx

    """
    For easier printing I have also overriden the str method to return the 
    State's index.
    """
    def __str__(self):
        return str(self.stateIdx)

# Configuration - tuple of State and string.
Conf = (State, str)

# The DFA
class DFA:
    def __init__(self, lines):
        # DFA input properties
        self.name: str = lines[1]
        self.alphabet: set[str] = set()
        self.initialState: State = None
        self.delta: Dict[int, Dict[str, State]] = dict()
        self.finalStates: set[State] = set()

        # Properties generated from the input properties
        self.sinkStates: set[State] = set()
        self.states: set[State] = set()

        """
        Initialise the inititial state from the input and the default 
        Sink State with -1.
        """
        self.initialState = State(lines[2].rstrip())
        self.sinkStates.add(State(-1))

        """
        The reversed Delta Function used for generating the Sink States. 

        It stores all the transitions from the delta function (dictionary) but 
        in revered form: 
            source -char-> destination => destination -char-> source.

        Also I've noticed that the reversed delta function matches an NFA 
        rather than a DFA (because there can be more transitions on the same 
        character from the same source) 
        """
        reversed_delta: Dict[int, Dict[str, List[State]]] = dict()

        # Split the alphabet into characters.
        for i in range(len(lines[0])):
            self.alphabet.add(lines[0][i])

        # Generate the delta function (dictionary)
        for line in lines[3:-1]:
            """
            Extract and parse the Current State, the Character and the 
            Next State.
            """
            cur_state, char, new_state = line.split(",")
            cur_state = State(cur_state)
            new_state = State(new_state.rstrip())
            char = char[1:-1]

            # If the state is not in the states set then add it.
            if cur_state not in self.states:
                self.states.add(cur_state)

            if new_state not in self.states:
                self.states.add(new_state)

            """
            If the Current (New) State is not in delta (reversed delta) then 
            add it.
            """
            if hash(cur_state) not in self.delta:
                self.delta[hash(cur_state)] = dict()

            if hash(new_state) not in reversed_delta:
                reversed_delta[hash(new_state)] = dict()

            """
            Add the transition from the Current State to the New State via the 
            Character to the delta function (dictionary).
            """
            self.delta[hash(cur_state)][char] = new_state

            """
            As I mentioned before the reversed delta function corresponds to 
            an NFA so first I check if there is already one transition from 
            the Current State via the Character and I add the transition to 
            the reversed delta function (dictionary) adequately.
            """
            if char in reversed_delta[hash(new_state)]:
                reversed_delta[hash(new_state)][char].append(cur_state)
            else:
                reversed_delta[hash(new_state)][char] = [cur_state]

        # Read and parse the Final States
        final_states = lines[-1].rstrip().split(" ")
        for stateIdx in final_states:
            self.finalStates.add(State(stateIdx))

        """
        Using the reversed delta function I do a BFS from each Final State and 
        generate all the reachable states from them.
        """
        reachable_states = set()
        for state in self.finalStates:
            reachable_states = reachable_states | self.bfs(state, reversed_delta)

        """
        The Sink States are:
        (DFA's States - Reachable States) + The Default Sink State
        """
        self.sinkStates = self.sinkStates | (self.states - reachable_states)

    # BFS
    @staticmethod
    def bfs(src: State, reversed_delta: Dict[int, Dict[str, State]]):
        # States left to visit
        states = [src]

        # States visited
        visited = [src]

        # Reachable states
        accessible_states = set()
        accessible_states.add(src)

        # Check all the states that can be reached.
        while len(states):
            state = states.pop(0)

            # If it isn't a self-transition add it to the accesible states.
            if hash(src) != hash(state):
                accessible_states.add(state)

            # Check if the state is in the dictionary.
            if hash(state) in reversed_delta:
                # Check the transitions on each Character (key).
                for key in reversed_delta[hash(state)].keys():
                    # Check all the transitions on the current Character.
                    for adj in reversed_delta[hash(state)][key]:
                        """
                        If it hasn't been visited already then add it to the 
                        list to visit and mark it as visited.
                        """
                        if adj not in visited:
                            visited.append(adj)
                            states.append(adj)

        return accessible_states

    # Execute one step on a Configuration. 
    def step(self, conf: Conf) -> Conf:
        # I'm using repr in order not to hardcode the '\n' character.
        char = repr(conf[1][0])[1:-1]

        """ 
        Check if there is any transition on the current Character from the 
        Current State in the dictionary.

        If there isn't then the DFA will reach a Sink State, else it will 
        reach the corresponding Next State.
        """
        if hash(conf[0]) not in self.delta or char not in self.delta[hash(conf[0])]:
            new_state = list(self.sinkStates)[0]
        else:
            new_state = self.delta[hash(conf[0])][char]

        return new_state, conf[1][1:]

    # Check if a word is accepted by the DFA (not used in the current version).
    def accept(self, word: str) -> bool:
        cur_state = self.initialState

        # Execute one step and check if a Sink State has been reached.
        while word != "":
            if cur_state in self.sinkStates:
                break
            cur_state, word = self.step((cur_state, word))

        # Check if a Final State has been reached.
        for state in self.finalStates:
            if state == cur_state:
                return True

        return False

# The DFA
class Lexer:
    def __init__(self, line, output):
        self.dfas: list = list()
        self.output: str = output

        # I used this list to store all the DFAs' current states.
        self.states: list = list()

        # Parse the in[ut for the DFAs and create each one.
        lines = ''.join(line)
        lines = lines.split("\n\n")

        for line in lines:
            dfa = DFA(line.split('\n'))
            self.dfas.append(dfa)
            self.states.append(dfa.initialState)

    def print_lexemes(self, lexemes, word, sinking_idx, sinked_dfas):
        with open(self.output, "w") as output:
            # The word has been tokenized successfully.
            if sinked_dfas != len(self.dfas):
                output.write(lexemes)
            else:
                # Find the line where all the DFAs sinked.
                line = 0
                line_breaks = [i for i, char in enumerate(word) if char == "\n"]

                for i in line_breaks:
                    if sinking_idx > i:
                        line += 1

                # Print the character's index and line on which all the DFAs have sinked.
                if sinking_idx == len(word):
                    output.write("No viable alternative at character EOF, line " + str(line))
                else:
                    output.write("No viable alternative at character " + str(sinking_idx) + ", line " + str(line))

    # Get the lexemes from a word using the DFAs.
    def get_lexemes(self, word):
        lexemes = ""
        left = 0
        right = 1

        # While the word hasn't been completely split in tokens.
        while left <= right <= len(word):
            """
            Start and End of the longest Accepted Token and the DFA which 
            accepts it.
            """
            max_left = max_right = -1
            max_dfa = None

            """
            Number of DFAs which have reached a Sink State and the character's index
            on which the last DFA has sinked.
            """
            sinked_dfas = 0
            sinking_idx = right - 1

            """
            Try to get the longest accepted token starting from left by each 
            DFA.

            If the character does not make the DFA reach a Sink State proceed 
            to test the next Character.
            """
            for dfa in self.dfas:
                aux_left = left
                aux_right = right

                # While the word's end hasn't been reached.
                while aux_right <= len(word):
                    # Execute one step.
                    new_state, _ = dfa.step((self.states[self.dfas.index(dfa)], word[aux_right - 1]))

                    """
                    Check if a Sink State has been reached and update the 
                    number of Sinked DFAs and the Sinking Index if necessary.
                    """
                    if new_state in dfa.sinkStates:
                        sinked_dfas += 1
                        sinking_idx = max(aux_right - 1, sinking_idx)
                        break

                    """
                    Check if the Character made the DFA accept the word and 
                    update the vars regarding the longest accepted word 
                    adequately.
                    """
                    if aux_right > max_right and new_state in dfa.finalStates:
                        max_left = aux_left
                        max_right = aux_right
                        max_dfa = dfa

                    # Update the DFA's state and move to the next Character.
                    self.states[self.dfas.index(dfa)] = new_state
                    aux_right += 1

                """
                If the word's end has been reached and neither a Final State, 
                nor a Sink State has been reached update the number of Sinked 
                DFAs and the Sinking Index.
                """
                if aux_right > len(word) and new_state not in (dfa.finalStates | dfa.sinkStates):
                    sinked_dfas += 1
                    sinking_idx = len(word)

                self.states[self.dfas.index(dfa)] = dfa.initialState

            """
            Update the Lexemes if possible, or check if all the DFAs have 
            sinked and the word can't be tokenized.
            """
            if max_dfa:
                lexemes += max_dfa.name + " " + repr(word[max_left:max_right])[1:-1] + "\n"
                left = max_right
                right = max_right + 1
            elif sinked_dfas == len(self.dfas):
                break

        self.print_lexemes(lexemes, word, sinking_idx, sinked_dfas)


def runlexer(lexerfinput, finput, foutput):
    # Read the Lexer
    with open(lexerfinput, "r") as input:
        dfas = input.readlines()
        lexer = Lexer(dfas, foutput)

    # Read the word and split it into Tokens.
    with open(finput, "r") as input:
        words = input.readlines()
        word = ''.join(words)
        lexer.get_lexemes(word)
