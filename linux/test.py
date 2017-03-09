from combos import n_distinguish_in_k as nk

blocks = ['a', 'a', 'b', 'c', 'd']

N_Spaces = 25
boards = nk(blocks, N_Spaces)

print boards[0]
