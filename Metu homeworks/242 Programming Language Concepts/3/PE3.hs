module PE3 where

-- can use these if you want to...
import Data.List
import Data.Maybe

data Private = Private { idNumber :: Int, height :: Int, timeToSwap :: Int } deriving Show

data Cell = Empty | Full Private deriving Show

type Area = [[Cell]] 

---------------------------------------------------------------------------------------------
------------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
--------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
--------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
---------------------------------------------------------------------------------------------


-- Note: undefined is a value that causes an error when evaluated. Replace it with
-- a viable definition! Name your arguments as you like by changing the holes: _


--------------------------
-- Part I: Warming up with Abstractions

-- bubbleHumans: Applying a parallel bubble step on integers.
bubbleHumans :: [Int] -> [Int]
bubbleHumans [] = []
bubbleHumans (a:[]) = [a]
bubbleHumans (a:b:c)
    | (a<b) = b:a: bubbleHumans(c)
    | otherwise = a:bubbleHumans(b:c)
    
bubbleHumans1 :: [Int]-> [Private] -> [Private]
bubbleHumans1 [] [] = []
bubbleHumans1 (a:[]) (a1:[]) = [a1]
bubbleHumans1 (a:b:c) (d:e:f)
    | (a<b) = e:d:bubbleHumans1 c f 
    | otherwise = d:bubbleHumans1 (b:c) (e:f) 
    
bubbleHumans11 :: [Int]-> [Private] -> [Private]
bubbleHumans11 [] [] = []
bubbleHumans11 (a:[]) (a1:[]) = [a1]
bubbleHumans11 (a:b:c) (d:e:f)
    | (a>b) = e:d:bubbleHumans11 c f
    | otherwise = d:bubbleHumans11 (b:c) (e:f) 

-- bubblePrivates: The same thing, but on privates with a key function and an asc/desc option.
bubblePrivates :: (Private -> Int) -> Bool -> [Private] -> [Private]
bubblePrivates which True priv = bubbleHumans1 [ which(l)  | l<- priv] priv 
bubblePrivates which False priv = bubbleHumans11 [ which(l)  | l<- priv] priv 

-- sortPrivatesByHeight: Full sort via multiple bubble steps, calculate the sorting time too!
sortPrivatesByHeight :: [Private] -> ([Private], Int)
sortPrivatesByHeight _ = undefined


--------------------------
-- Part II: Squeezing your Brain Muscles

-- ceremonialFormation: Sorting again, but with multiple files. Don't forget the time!
ceremonialFormation :: Area -> (Area, Int)
ceremonialFormation _ = undefined

-- swapPrivates: Swap two arbitrary privates by ID if they are in the area. Big ouch!
swapPrivates :: Int -> Int -> Area -> Area
swapPrivates _ _ _ = undefined 

-- Best of luck to you, friend and colleague!

