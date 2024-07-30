import os
import sys
import zlib

# calling strace -f ./topo_order_commits.py we get an error with a return of -1
# this proves no git operations were called in this program

class CommitNode:
    def __init__(self, commit_hash):
        self.commit_hash = commit_hash
        self.parents = set()
        self.children = set()


# Finds path of top level git directory 
def find_git():

    while True:
        # get current directory
        dir = os.getcwd()

        # check if .git exists in current directory
        if '.git' in os.listdir():
            # return path to .git dir
            return os.path.join(dir, '.git')
        
        # if dir is '/' directory and .git not found, 
        if dir == '/':
            sys.stderr.write('Not inside a Git repository')
            exit(1)

        # go to parent directory if still looking
        os.chdir('../')


# gets the list of local branch names found in .git/refs/heads
def get_branches(head_dir, rel_dir, hash_list, branches):

    # iterate through each file / directory in specified directory
    for item in os.scandir(rel_dir):
        # get relative path from head directory to the file / dir
        file = os.path.relpath(item.path, head_dir)

        # element is directory (not branch)
        if item.is_dir():
            # use recursion to iterate through nested dictionary
            get_branches(head_dir, item.path, hash_list, branches)
        
        # element is file branch (branch)
        elif item.is_file():
            # open file
            fopen = open(head_dir + '/' + file, "r")
            # get commit hash from first line
            hash = fopen.readline().strip()
            # store hash in list of all commit hashes
            hash_list.append(hash)
            # store commit hash in dictionary key mapping each hash to list of files
            if hash in branches.keys():
                branches[hash].append(file)
            else:
                branches[hash] = [file]
            # close file
            fopen.close()


# helper function to get parents of a child node
def get_parents(object_dir, hash):
    # get filepath of hash file in objects directory
    filepath = object_dir + '/' + hash[:2] + '/' + hash[2:]
    # open file and read its contents
    fopen = open(filepath, 'rb').read()
    # decompress the file and split each line into an array
    lines = zlib.decompress(fopen).decode().split('\n')

    parents = set()
    for line in lines:
        # split words into an array
        words = line.split()
        if len(words) > 0:
            # for each line with parent tag, add info following tag to parents object
            for word in words:
                if words[0] == 'parent' and word != words[0]:
                    parents.add(word)

    return parents


# builds commit graph where each commit is represented as an instance of the CommitNode class
def build_commit_graph(object_dir, hash_list):
    # commit graph 
    graph = {}
    # union of all the leaf nodes across all the branches
    root_commits = list()
    # stack of objects to traverse for depth-first search traversal
    stack = hash_list
    # track visited nodes
    visited = set()     

    # iterate until stack empty
    while len(stack) != 0:
        # get commit hash of an object
        hash = stack.pop()

        # if hash not processed yet
        if hash not in visited:
            # add hash to root commits
            visited.add(hash)
            # create commitnode object if hash doesn't have one already
            if hash not in graph.keys():
                graph[hash] = CommitNode(hash)
            # get that commitnode object 
            node = graph[hash]
            # get its parents using helper function to add the parent hash to the child node’s parents
            node.parents = get_parents(object_dir, node.commit_hash)

            # if parent a leaf node, add to root_commits
            if len(node.parents) == 0:
                root_commits.append(node.commit_hash)

            # iterate for each parent in parents
            for parent in node.parents:
                # add parent to graph 
                if parent not in graph.keys():
                    graph[parent] = CommitNode(parent)
                # add parent to root commits
                if parent not in visited:
                    stack.append(parent)
                # add the child hash to the parent node’s children to create parent-child relationship
                graph[parent].children.add(node.commit_hash)

            # update hash object in the graph
            graph[hash] = node

    return root_commits, graph


# get topological ordering of commits in the graph
def get_topo_order(root_commits, commit_graph):
    # stores sorted topological ordering
    sorted_order = list()
    # stores nodes with permanent mark
    perm_stack = root_commits
    # stores nodes with temporary marks
    temp_stack = list()
    # track visited nodes
    visited = set()

    # iterate until permanent stack is empty
    while len(perm_stack) != 0:
        # get node
        node = perm_stack.pop()

        if node not in visited:
            # add node to visted
            visited.add(node)

            # iterate through node's children
            for child in commit_graph[node].children:
                # add to permanent marker if not visited
                if child not in visited:
                    perm_stack.append(child)

            # iterate while temporary marks on for each node with an an edge 
            while len(temp_stack) != 0 and node not in commit_graph[temp_stack[-1]].children:
                sorted_order.append(temp_stack.pop())

            # place temporary mark on node
            temp_stack.append(node)

    # add rest of temp_stack in preceding order
    for node in reversed(temp_stack):
        sorted_order.append(node)

    return sorted_order

# print commit hashes in the order generated by the topological ordering, from the least to the greatest
def print_topo_order(branches, commit_graph, topo_ordered_commits):
    cont = False

    for hash in topo_ordered_commits:
        # get commitnode object
        node = commit_graph[hash]
        # perform sticky start
        if cont:
            print('=' + ' '.join(node.children))
            cont = False
        
        # get branches 
        if hash in branches:
            print(hash + ' ' + ' '.join(sorted(branches[hash])))
        else:
            print(hash)

        ind = topo_ordered_commits.index(hash)+1
        if len(topo_ordered_commits) > ind:
            # perform sticky end
            if topo_ordered_commits[ind] not in node.parents:
                print(' '.join(node.parents) + '=\n')
                cont = True
    

def topo_order_commits():
    git_path = find_git()

    head_dir = git_path + '/refs/heads'
    commit_hashes = []
    local_branches = {}
    get_branches(head_dir, head_dir, commit_hashes, local_branches)

    object_dir = git_path + '/objects'
    root_commits, commit_graph = build_commit_graph(object_dir, commit_hashes)

    topo_ordered_commits = get_topo_order(root_commits, commit_graph)
    print_topo_order(local_branches, commit_graph, topo_ordered_commits)


if __name__ == '__main__':
    topo_order_commits()