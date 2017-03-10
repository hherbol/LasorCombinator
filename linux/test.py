from combos import n_distinguish_in_k as nk

blocks = ['a', 'a', 'b', 'c', 'd']

N_Spaces = 16
boards = nk(blocks, N_Spaces)

print len(boards)

