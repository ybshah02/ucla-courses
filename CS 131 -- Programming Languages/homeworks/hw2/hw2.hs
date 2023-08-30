
-- problem 1 --

-- a

scale_nums:: [Integer] -> Integer -> [Integer]
scale_nums nums scaler = map(\num -> num * scaler) nums

-- b
only_odds:: [[Integer]] -> [[Integer]]
only_odds nums = filter (\list -> all odd list) nums

-- c

largest:: String->String->String
largest a b
    | length a >= length b = a
    | otherwise = b

largest_in_list:: [String] -> String
largest_in_list words = foldl largest "" words

-- problem 2 --

-- a

count_if:: (a->Bool) -> [a] -> Integer
count_if _ [] = 0
count_if predfunc (x:xs)
    | predfunc x = 1 + count_if predfunc xs
    | otherwise = count_if predfunc xs

-- c

count_if_with_fold:: (a->bool) -> [a] -> Integer
count_if_with_fold predfunc xs = foldl (\acc x -> if predfunc xs then acc + 1 else acc) 0 xs


-- problem 7 --

data LinkedList = EmptyList | ListNode Integer LinkedList 
    deriving Show

ll_contains:: LinkedList -> Integer -> Bool
ll_contains EmptyList num = False
ll_contains (ListNode val next) num = val == num || ll_contains next num

ll_insert :: LinkedList -> Integer -> Integer -> LinkedList
ll_insert list index value
  | index <= 0 = ListNode value list
ll_insert (ListNode v next) index value
  | index == 1 = ListNode v (ListNode value next)
  | otherwise  = ListNode v (ll_insert next (index - 1) value)
ll_insert EmptyList _ value = ListNode value EmptyList

-- problem 8 --

longest_run :: [Bool] -> Int
longest_run = fst . foldr step (0, 0)
  where
    step b (maxr, curr) =
      if b
        then let new = curr + 1 in (max new maxr, new)
        else (maxr, 0)

data Tree = Empty | Node Integer [Tree]

max_tree_value :: Tree -> Integer
max_tree_value Empty = 0
max_tree_value (Node val children) = val `max` (foldr (max . max_tree_value) 0 children)
