import sys
import re

def read_productions(productions):
    file_name = str(sys.argv[1]) if len(sys.argv) > 1 else "input"
    f = open(file_name)

    for l in f:
        left, right = tuple(x.strip() for x in l.split('->'))
        productions[left] = [x.strip() for x in right.split('|')]

    f.close();


def lambda_production_removal(productions):

    done = False;

    while not done:

        done = True

        key_with_lambda = None

        for key in productions:
            if '$' in productions[key]:
                done = False
                key_with_lambda = key

        if key_with_lambda:
            if len(productions[key_with_lambda]) == 1:
                del productions[key_with_lambda]
                for key in productions:
                    for i in range(len(productions[key])):
                        productions[key][i] = productions[key][i].replace(key_with_lambda, '')
                        if productions[key][i] == '':
                            productions[key][i] = '$'
            else:
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
                        if len(elem) > 1 and key_with_lambda in elem:
                            aux.append(elem.replace(key_with_lambda, '', 1))
                    productions[key].extend(aux)


def transform(productions):
    lambda_production_removal(productions)


productions = {}
read_productions(productions)

print(productions)

transform(productions);

print(productions)
