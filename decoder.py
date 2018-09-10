#! /usr/bin/env python
'''
Hamming encoding and decoding for GLONASS as specified in the GLONASS ICD,
Edition 5.1, Section 4.7.
'''

import random
import itertools
import math
import numpy as np


class GloHammingDecoder:
    '''
    Decoder for the GLONASS hamming encoded data
    '''
    def __init__(self, input_bits):
        self.data_bits = input_bits
        self.checksums = [[None] * 7, [None]]

    index_set_i = [9, 10, 12, 13, 15, 17, 19, 20, 22, 24, 26, 28, 30, 32,
                   34, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59,
                   61, 63, 65, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84]
    index_set_j = [9, 11, 12, 14, 15, 18, 19, 21, 22, 25, 26, 29, 30, 33,
                   34, 36, 37, 40, 41, 44, 45, 48, 49, 52, 53, 56, 57, 60,
                   61, 64, 65, 67, 68, 71, 72, 75, 76, 79, 80, 83, 84]
    index_set_k = [10, 11, 12, 16, 17, 18, 19, 23, 24, 25, 26, 31, 32, 33, 34,
                   38, 39, 40, 41, 46, 47, 48, 49, 54, 55, 56, 57, 62, 63, 64,
                   65, 69, 70, 71, 72, 77, 78, 79, 80, 85]
    index_set_l = [13, 14, 15, 16, 17, 18, 19, 27, 28, 29, 30, 31, 32, 33, 34,
                   42, 43, 44, 45, 46, 47, 48, 49, 58, 59, 60, 61, 62, 63, 64,
                   65, 73, 74, 75, 76, 77, 78, 79, 80]
    index_set_m = [20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
                   50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
                   63, 64, 65, 81, 82, 83, 84, 85]

    def calculate_checksums(self):
        """Calculates the Hamming checksums for GLONASS string

        This algorithm was extracted from the GLONASS ICD, Edition 5.1, Section
        4.7. The variables used here correspond to the variables used in the
        ICD.
        """

        c_sub_1 = 0
        for index in self.index_set_i:
            c_sub_1 = c_sub_1 ^ bool(self.data_bits[85-index])
        c_sub_1 = c_sub_1 ^ bool(self.data_bits[84])

        c_sub_2 = 0
        for index in self.index_set_j:
            c_sub_2 = c_sub_2 ^ bool(self.data_bits[85-index])
        c_sub_2 = c_sub_2 ^ bool(self.data_bits[83])

        c_sub_3 = 0
        for index in self.index_set_k:
            c_sub_3 = c_sub_3 ^ bool(self.data_bits[85-index])
        c_sub_3 = c_sub_3 ^ bool(self.data_bits[82])

        c_sub_4 = 0
        for index in self.index_set_l:
            c_sub_4 = c_sub_4 ^ bool(self.data_bits[85-index])
        c_sub_4 = c_sub_4 ^ bool(self.data_bits[81])

        c_sub_5 = 0
        for index in self.index_set_m:
            c_sub_5 = c_sub_5 ^ bool(self.data_bits[85-index])
        c_sub_5 = c_sub_5 ^ bool(self.data_bits[80])

        c_sub_6 = 0
        for index in range(50, 19, -1):
            c_sub_6 = c_sub_6 ^ bool(self.data_bits[index])
        c_sub_6 = c_sub_6 ^ bool(self.data_bits[79])

        c_sub_7 = 0
        for index in range(19, 0, -1):
            c_sub_7 = c_sub_7 ^ bool(self.data_bits[index])
        c_sub_7 = c_sub_7 ^ bool(self.data_bits[78])
        c_sub_7 = c_sub_7 ^ bool(self.data_bits[0])

        c_sub_sigma = 0
        for bit in self.data_bits:
            c_sub_sigma = c_sub_sigma ^ bool(bit)

        self.checksums = [[c_sub_1, c_sub_2, c_sub_3, c_sub_4, c_sub_5, c_sub_6,
                           c_sub_7], c_sub_sigma]

        print(self.checksums)

    def print_checksums(self):
        ''' Prints checksums'''
        print("CHECKSUMS:")
        for checksum in self.checksums[0]:
            print(checksum)

        print("c_sub_sigma: {}".format(self.checksums[1]))

    def decode(self):
        ''' Deocde the input bits.

        Calculates checksums and checks them to verify the data. Up to one
        corrupted bit is fixed yielding valid data.

        Output:
            True when data is not corrupted, False otherwise.
        '''
        self.calculate_checksums()
        return self.verify_data()

    def check_cond_a(self):
        ''' Check whether condition a) from the ICD is satisfied

        Checks whether condition a) from the GLONASS ICD, Edition 5.1, Section
        4.7 is satisfied. Note this this method required the checksums to be
        calculated.

        Output:
            True when condition a) is satisfier, False otherwise
        '''
        checksums = self.checksums[0]
        c_sub_sigma = self.checksums[1]

        if checksums.count(0) == len(checksums) and c_sub_sigma == 0:
            return True

        if checksums.count(1) == 6 and c_sub_sigma == 1:
            return True

        return False

    def verify_data(self):
        ''' Check whether the input data is valid. Fix it if recoverable.

        Checks whether the input data is valid. Extracted from the GLONASS ICD,
        Edition 5.1, Section 4.7. The coding scheme used in GLONASS allows one
        to fix up to 1 corrupted bit. With this in mind, one corrupted bit does
        not invalidate the data. This method will fix the corrupted bit instead
        and re-verify accordingly. Note that it requires the checksums to be
        calculated.

        Output:
            True when data is valid, False otherwise.
        '''
        checksums = self.checksums[0]
        c_sub_sigma = self.checksums[1]

        if self.check_cond_a():
            return True

# Condition b) from GLONASS ICD, Edition 5.1, Section 4.7 (there's one
        # corrupted bit that can be fixed)
        if (c_sub_sigma == 1 and checksums.count(1) >= 2):
            # Calculate K from the GLONASS ICD
            checksums.reverse()
            K = 7 - checksums.index(1)
            checksums.reverse()

            # Calculate the position of the bit to be fixed
            bit_position = 0
            for index, val in enumerate(checksums):
                # Note that `val` is either 0 or 1
                bit_position = bit_position + val * pow(2, index)
            bit_position = bit_position + 8 - K
            print("bit_position = {}".format(bit_position))

            if bit_position > 85:
                return False

            # Fix the bit
            self.data_bits[85 - bit_position] = int(
                bool(self.data_bits[85 - bit_position]) ^ True)
            self.calculate_checksums()

            # Verify that indeed condition a) from the ICD is now satisfied.
            if self.check_cond_a():
                return True

            return False

        # Condition c) from the GLONASS ICD, Edition 5.1, Section 4.7 (more
        # than one corrupted bit, data is not recoverable)
        if (c_sub_sigma == 1 and checksums.count(0) == 0) or \
                (c_sub_sigma == 0 and checksums.count(1) >= 2):
            return False

        return False


class GloHammingEncoder:
    '''
    Encoder for the GLONASS string
    '''
    def __init__(self, input_bits):
        self.data_bits = input_bits
        self.parity_bits = [None] * 8

    index_set_i = [9, 10, 12, 13, 15, 17, 19, 20, 22, 24, 26, 28, 30, 32,
                   34, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59,
                   61, 63, 65, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84]
    index_set_j = [9, 11, 12, 14, 15, 18, 19, 21, 22, 25, 26, 29, 30, 33,
                   34, 36, 37, 40, 41, 44, 45, 48, 49, 52, 53, 56, 57, 60,
                   61, 64, 65, 67, 68, 71, 72, 75, 76, 79, 80, 83, 84]
    index_set_k = [10, 11, 12, 16, 17, 18, 19, 23, 24, 25, 26, 31, 32, 33, 34,
                   38, 39, 40, 41, 46, 47, 48, 49, 54, 55, 56, 57, 62, 63, 64,
                   65, 69, 70, 71, 72, 77, 78, 79, 80, 85]
    index_set_l = [13, 14, 15, 16, 17, 18, 19, 27, 28, 29, 30, 31, 32, 33, 34,
                   42, 43, 44, 45, 46, 47, 48, 49, 58, 59, 60, 61, 62, 63, 64,
                   65, 73, 74, 75, 76, 77, 78, 79, 80]
    index_set_m = [20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
                   50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
                   63, 64, 65, 81, 82, 83, 84, 85]

    def encode(self):
        '''Encodes the data associated with this instance'''
        bits_out = np.zeros(85)
        bits_out[0:77] = self.data_bits

        for index in self.index_set_i:
            self.parity_bits[7] = int(bool(self.data_bits[85-index])
                                  ^ bool(self.parity_bits[7]))

        for index in self.index_set_j:
            self.parity_bits[6] = int(bool(self.data_bits[85-index])
                                  ^ bool(self.parity_bits[6]))

        for index in self.index_set_k:
            self.parity_bits[5] = int(bool(self.data_bits[85-index])
                                  ^ bool(self.parity_bits[5]))

        for index in self.index_set_l:
            self.parity_bits[4] = int(bool(self.data_bits[85-index])
                                  ^ bool(self.parity_bits[4]))

        for index in self.index_set_m:
            self.parity_bits[3] = int(bool(self.data_bits[85-index])
                                  ^ bool(self.parity_bits[3]))

        for index in range(35, 67):
            self.parity_bits[2] = int(bool(self.data_bits[85-index])
                                  ^ bool(self.parity_bits[2]))

        for index in range(66, 86):
            self.parity_bits[1] = int(bool(self.data_bits[85-index])
                                  ^ bool(self.parity_bits[1]))

        self.parity_bits[0] = 0
        for val in self.data_bits:
            self.parity_bits[0] = int(bool(self.parity_bits[0]) ^ bool(val))

        for val in self.parity_bits:
            self.parity_bits[0] = int(bool(self.parity_bits[0]) ^ bool(val))

        self.print_parity_bits()

        bits_out[77:85] = self.parity_bits

        return bits_out

    def print_parity_bits(self):
        ''' Prints parity bits'''
        print("PARITY BITS:")
        for bit in self.parity_bits:
            print(bit)

class GalConvCode:
    def __init__(self, input_bits, invert_second_branch = False):
        # As per Galilelo ICD (p. 24, Figure 13), the second branch of the
        # encoder is inverted. Use this parameter to set the inversion
        # on and off.
        self.invert_second_branch = invert_second_branch
        self.data_bits = input_bits
        self.output_bits = np.zeros(2*len(input_bits))

    # The generator polynomials as specified in the Galilelo ICD, p. 26, Table
    # 23.
    G1 = [1, 1, 1, 0, 0, 1]
    G2 = [0, 1, 1, 0, 1, 1]

    def poly_eval(self, state, bit):
        out_g1 = bit
        for index_g1, val in enumerate(state):
            if 1 == self.G1[index_g1]:
                out_g1 = int(bool(val) ^ bool(out_g1))

        out_g2 = bit
        for index_g2, val in enumerate(state):
            if 1 == self.G2[index_g2]:
                out_g2 = int(bool(val) ^ bool(out_g2))

        return (out_g1, out_g2)

    def encode(self):
        prev_val = [0] * 6

        for index, input_val in enumerate(self.data_bits):
            out1, out2 = self.poly_eval(prev_val, input_val)
            self.output_bits[2*index] = out1
            self.output_bits[2*index+1] = out2

            prev_val.pop()
            prev_val.insert(0, input_val)

        if self.invert_second_branch:
            for ind, sym in enumerate(self.output_bits):
                if ind % 2 == 1:
                    self.output_bits[ind] = int(not bool(self.output_bits[ind]))

        f = lambda x:-1 if x == 1 else 1
        self.output_bits = [f(x) for x in self.output_bits]

    def decode(self, msg):
        f = lambda x:1 if x == -1 else 0
        msg = [f(x) for x in msg]
        print(msg)

        class TrelisNode:
           def  __init__(self, visited, error, ancestor):
                self.visited = visited
                self.error = error
                self.ancestor = ancestor

           def __str__(self):
               return "".join([str(self.visited), ' {:^3} '.format(str(self.error)), str(self.ancestor)])


        trelis = [[TrelisNode(0, math.inf, 0) for n in range(64)] for nn in range(int(len(msg)/2 + 1))]
        # trelis = [[TrelisNode(0, math.inf, 0) for n in range(64)] for nn in range(10)]
        trelis[0][0].visited = 1

        # states = [[].append(seq) for seq in itertools.product([0,1], repeat=6)]
        states_str = ["".join(seq) for seq in itertools.product("01", repeat=6)]
        states = [list(word) for word in states_str]

        for ii, column in enumerate(trelis[:-1]):
            for current_state_idx, node in enumerate(column):
                if 1 == node.visited:
                    # THESE INDICES ARE WRONG!!!
                    idx_1 = 2*ii
                    idx_2 = 2*ii + 1

                    state_temp = [int(n) for n in states[current_state_idx]]
                    out = self.poly_eval(state_temp, 0)
                    error = abs(msg[idx_1] - out[0]) + abs(msg[idx_2] - out[1])
                    # print("MESSAGE {} {}".format(msg[idx_1], msg[idx_2]))

                    # The affected state
                    state_new_zero = states[current_state_idx][:-1]
                    state_new_zero.insert(0, '0')
                    next_state_idx = states.index(state_new_zero)

                    prev_error = 0 if trelis[ii][current_state_idx].error == math.inf else trelis[ii][current_state_idx].error
                    if (error + prev_error) < trelis[ii+1][next_state_idx].error:
                        trelis[ii+1][next_state_idx].visited = 1
                        trelis[ii+1][next_state_idx].ancestor = current_state_idx
                        trelis[ii+1][next_state_idx].error = error + prev_error
                        # print("COLUMN: {} CURRENT STATE: {} NEXT STATE: {} ERROR: {} STATE: {}".format(ii, current_state_idx, next_state_idx, trelis[ii+1][next_state_idx].error, state_new_zero))

                    state_temp = [int(n) for n in states[current_state_idx]]
                    out = self.poly_eval(state_temp, 1)
                    error = abs(msg[idx_1] - out[0]) + abs(msg[idx_2] - out[1])

                    # The affected state
                    state_new_one = states[current_state_idx][:-1]
                    state_new_one.insert(0, '1')
                    next_state_idx = states.index(state_new_one)

                    prev_error = 0 if trelis[ii][current_state_idx].error == math.inf else trelis[ii][current_state_idx].error
                    if (error + prev_error) < trelis[ii+1][next_state_idx].error:
                        trelis[ii+1][next_state_idx].visited = 1
                        trelis[ii+1][next_state_idx].ancestor = current_state_idx
                        trelis[ii+1][next_state_idx].error = error + prev_error
                        # print("COLUMN: {} CURRENT STATE: {} NEXT STATE: {} ERROR: {} STATE: {}".format(ii, current_state_idx, next_state_idx, trelis[ii+1][next_state_idx].error, state_new_one))

        values = [value for value in trelis[len(trelis)-1] if value.error == 0]
        idx_start = trelis[len(trelis)-1].index(values[0])
        print(values)
        print("TATATATATA")
        msg_tx = []
        print(msg_tx)
        for column in reversed(trelis[1:]):
            print(states[idx_start][0], idx_start)
            # TODO Better deal with int vs char
            msg_tx.insert(0, int(states[idx_start][0]))
            idx_start = column[idx_start].ancestor
        column = trelis[len(trelis)-1]
        # for item in column:
        #     print(item.error)
        # print(msg_tx)
        # print(len(msg_tx))
        return(msg_tx)

def func():
    data_in = [1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1,
            0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1,
            1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0]
    # data_in = [ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0]
    encoder = GalConvCode(data_in)
    encoder.encode()
    print(encoder.output_bits)

    encoder.decode(encoder.output_bits)

if __name__ == "__main__":
    func()
