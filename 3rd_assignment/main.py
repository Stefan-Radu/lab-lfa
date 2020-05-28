import sys
import re

def read_productions(productions):
    file_name = str(sys.argv[1]) if len(sys.argv) > 1 else "input"
    f = open(file_name)

    for l in f:
        left, right = tuple(x.strip() for x in l.split('->'))
        productions[left] = [x.strip() for x in right.split('|')]

    f.close();
    print('Before: ', productions)


def get_new_productions(production, key):

    poz = []
    for i in range(len(production)):
        if production[i] == key:
            poz.append(i)

    poz.reverse()

    ret = []
    for mask in range(1 << len(poz)):
        aux = production
        for bit in range(len(poz)):
            if mask & (1 << bit):
                aux = aux[:poz[bit]] + aux[poz[bit] + 1:]
        if aux == '':
            aux = '$'

        ret.append(aux)

    return ret


def lambda_production_removal(productions):

    done = False

    while not done:

        done = True
        key_with_lambda = None

        for key in productions:
            if '$' in productions[key]:
                done = False
                key_with_lambda = key
                break

        if key_with_lambda:
            if len(productions[key_with_lambda]) == 1:
                # first case
                del productions[key_with_lambda]
                for key in productions:
                    for i in range(len(productions[key])):
                        productions[key][i] = productions[key][i].replace(key_with_lambda, '')
                        if productions[key][i] == '':
                            productions[key][i] = '$'
            else:
                # second case
                aux = []
                for i in range(len(productions[key_with_lambda])):
                    if productions[key_with_lambda][i] == '$':
                        aux.append(i)
                aux.reverse()
                for i in aux:
                    del productions[key_with_lambda][i]

                for key in productions:
                    aux.clear()
                    for elem in productions[key]:
                        # if len(elem) > 1 and key_with_lambda in elem:
                        #     aux.append(elem.replace(key_with_lambda, '', 1))
                        if key_with_lambda in elem:
                            aux.extend(get_new_productions(elem, key_with_lambda))
                    productions[key].extend(aux)
                    productions[key] = list(set(productions[key]))
    print('After step 1: ', productions)

def renamed_production_removal(productions):

    # trivial renames
    for key in productions:
        if key in productions[key]:
            productions[key].remove(key)

    # chained renames

    done = False

    while not done:

        done = True
        key1, key2 = None, None

        for key in productions:
            for item in productions[key]:
                if item in productions:
                    done = False
                    key1, key2 = key, item

        if done:
            break

        productions[key1].remove(key2)
        for item in productions[key2]:
            if item not in productions[key1]:
                productions[key1].append(item)

    # duplicates removal

    # to_remove = set()
    #
    # for key1 in productions:
    #     for key2 in productions:
    #         if key2 == 'S' or key1 == key2:
    #             continue
    #         if set(productions[key1]) == set(productions[key2]):
    #             to_remove.add(key2)
    #
    # for key in to_remove:
    #     del productions[key]
    print('After step 2: ', productions)


def endless_productions_dfs(t, productions, used):

    aux = []
    used.add(t)
    for production in productions[t]:
        ok1 = True
        for c in production:
            if c in used:
                aux.append(production)
                break
            if c.isupper():
                endless_productions_dfs(c, productions, used)


def useless_production_removeal(productions):

    used = set()
    endless_productions_dfs('S', productions, used)

    aux = []
    for key in productions:
        if key not in used:
            aux.append(key)

    for key in productions:
        to_del = True
        for production in productions[key]:
            if key not in production:
                to_del = False
                break
        if to_del:
            aux.append(key)

    for key in aux:
        del productions[key]

    for key in productions:
        for i in range(len(productions[key])):
            for k in aux:
                productions[key][i] = productions[key][i].replace(k, '')
        productions[key] = list(set(productions[key]))
    print('After step 3: ', productions)


def add_terminal_productions(next_letter, productions):

    to_be_added = set()
    for key in productions:
        for production in productions[key]:
            for c in production:
                if c.islower() and len(production) > 1:
                    to_be_added.add(c)

    for terminal in to_be_added:
        while next_letter in productions:
            next_letter = chr(ord(next_letter) + 1)
        for key in productions:
            for i in range(len(productions[key])):
                if len(productions[key][i]) > 1:
                    productions[key][i] = productions[key][i].replace(terminal, next_letter)
        productions[next_letter] = [terminal]
    print('After step 4: ', productions)


def add_non_terminal_productions(next_letter, productions):

    done = False

    while not done:

        done = True
        non_terminal = ''
        for key in productions:
            for production in productions[key]:
                if len(production) > 2:
                    done = False
                    non_terminal = production[1:]
                    break
            if not done:
                break

        if done:
            break

        while next_letter in productions:
            next_letter = chr(ord(next_letter) + 1)

        for key in productions:
            for i in range(len(productions[key])):
                if len(productions[key][i]) > 2:
                    productions[key][i] = productions[key][i].replace(non_terminal, next_letter)
        productions[next_letter] = [non_terminal]
    print('Final: ', productions)


def transform(productions):
    lambda_production_removal(productions)
    renamed_production_removal(productions)
    useless_production_removeal(productions)
    next_letter = 'A'
    add_terminal_productions(next_letter, productions)
    add_non_terminal_productions(next_letter, productions)


productions = {}
read_productions(productions)

transform(productions);
