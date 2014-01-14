							  using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;

namespace Game_FMG.Scripts
{
    class CollisionHelper
    {
        private List<GameObject> gameObjectList;

        public CollisionHelper(List<GameObject> gameObjectListI)
        {
            gameObjectList = gameObjectListI;
        }
        public CollisionHelper()
        {

        }
        //Public methods used to detect complete collisions
        #region//Check if a box is partially within another box
        public bool BoxHitsBox(CollisionBox box1, CollisionBox box2, bool equalSides)
        {
            //Create a list with all the points which need to be checked for collision
            List<Vector3> boxVectList = CreateBoxVectList(box1);

            //Value to know whether there has been a hit
            bool hit = false;

            //Looping to check 
            for (int i = 0; i < boxVectList.Count; i++)
            {
                //Check if a point is inside the given box
                if (VectorInBox(boxVectList[i], box2, equalSides))
                {
                    //There has been a hit
                    hit = true;
                }
            }

            //If there has been a hit, return true
            return hit;
        }
        #endregion
        #region//Check if a box is completely within another box
        public bool BoxInBox(CollisionBox box1, CollisionBox box2, bool equalSides)
        {
            //Check if the minimum and maximum are within the given box. If so, box1 
            //is completely inside box2
            bool hit = false;
            if (VectorInBox(box1.min, box2, equalSides))
            {
                if (VectorInBox(box1.max, box2, equalSides))
                {
                    hit = true;
                }
            }
            return hit;
        }
        #endregion
        #region//Check if a Vector3 is within box
        public bool VectorInBox(Vector3 vector, CollisionBox box, bool equalSides)
        {
            bool hit = false;
            //Check if the vector is bitween the box's min&max
            if (isBetweenWidth(vector, box.min, box.max, equalSides))
            {
                if (isBetweenHeight(vector, box.min, box.max, equalSides))
                {
                    if (isBetweenLength(vector, box.min, box.max, equalSides))
                    {
                        if (vector.Z < box.max.Z)//Is niet netjes maar kan ff niet anders
                        {
                            hit = true;
                        }
                    }
                }
            }
            return hit;
        }
        #endregion
        //Public method used to detect complete collision without boxes needed to be given
        #region//Check if a Vector3 is within box
        public bool VectorInBoxlistExcept(Vector3 vector, GameObject gameObject, bool equalSides)
        {
            bool hit = false;
            for (int i = 0; i < gameObjectList.Count; i++)
            {
                //Just for the easy linkage
                CollisionBox box = gameObjectList[i].collisionBox;

                if (gameObject != gameObjectList[i])
                {
                    //Check all axis
                    if (isBetweenWidth(vector, box.min, box.max, equalSides))
                    {
                        if (isBetweenHeight(vector, box.min, box.max, equalSides))
                        {
                            if (isBetweenLength(vector, box.min, box.max, equalSides))
                            {
                                if (vector.Z < box.max.Z)//Is niet netjes maar kan ff niet anders
                                {
                                    hit = true;
                                }
                            }
                        }
                    }
                }
            }
            return hit;
        }
        #endregion

        //To check and react on a collision
        #region//To check collision
        public void GameObjectCollidesCheck(GameObject gameObject, List<GameObject> list)
        {
            //Setting new position
            gameObject.previousPosition = gameObject.position;
            gameObject.previousVelocity = gameObject.velocity;

            //Adding the effects
            gameObject.velocity += gameObject.gravity;

            //Calculate new position
            Vector3 amountMoved = new Vector3();
            amountMoved.X = gameObject.velocity.X * (gameObject.time + 0.5f * (gameObject.time * gameObject.time));
            amountMoved.Y = gameObject.velocity.Y * (gameObject.time + 0.5f * (gameObject.time * gameObject.time));
            amountMoved.Z = gameObject.velocity.Z * (gameObject.time + 0.5f * (gameObject.time * gameObject.time));

            //Move to the calculated position
            gameObject.position += amountMoved;

            //Loop through every boundingbox inside the Object's list
            for (int i = 0; i < list.Count; i++)
            {
                //Make sure you don't check the player's collisionbox
                if (gameObject.name != list[i].name)
                {
                    //Check whether the position is in the collisionbox
                    if (VectorInBox(gameObject.position, list[i].collisionBox, true))
                    {
                        //Check from what side the box is hitted
                        CheckHit(gameObject, list[i].collisionBox);
                        //React on the info from above
                        HitReact(gameObject);
                    }
                }
            }
        }
        #endregion
        #region//Check at what side has the hit accurred
        private void CheckHit(GameObject gameObject, CollisionBox box)//miss nog die equals sides
        {
            if (VectorInBox(gameObject.position, box, true))
            {
                //Resetting the hit code
                gameObject.hit = "";

                //figuring out whether hit or not
                #region//if the current position is in front of the object
                if (VectorFrontBox(gameObject.previousPosition, box, true, false))
                {
                    gameObject.hit = "front";
                }
                else
                #endregion
                    #region//if the current position is behind the object
                    if (VectorBackBox(gameObject.previousPosition, box, true, false))
                    {
                        gameObject.hit = "back";
                    }
                    else
                    #endregion
                        #region//if the current position is above the object
                        if (VectorTopBox(gameObject.previousPosition, box, true, false))
                        {
                            gameObject.hit = "top";
                        }
                        else
                        #endregion
                            #region//if the current position is below the object
                            if (VectorUnderBox(gameObject.previousPosition, box, true, false))
                            {
                                gameObject.hit = "bottom";
                            }
                            else
                            #endregion
                                #region//if the current position is right the object
                                if (VectorRightBox(gameObject.previousPosition, box, true, false))
                                {
                                    gameObject.hit = "right";
                                }
                                else
                                #endregion
                                    #region//if the current position is left the object
                                    if (VectorLeftBox(gameObject.previousPosition, box, true, false))
                                    {
                                        gameObject.hit = "left";
                                    }
                                    #endregion

            }

        }
        #endregion
        #region//Reacting of the side that is hit
        private void HitReact(GameObject gameObject)
        {
            switch (gameObject.hit)
            {
                case "left":
                    //Properties set left
                    gameObject.position.X = gameObject.previousPosition.X;
                    gameObject.velocity.X = 0f;
                    break;
                case "right":
                    //Properties set right
                    gameObject.position.X = gameObject.previousPosition.X;
                    gameObject.velocity.X = 0f;
                    break;
                case "front":
                    //Properties set front
                    gameObject.position.Z = gameObject.previousPosition.Z;
                    gameObject.velocity.Z = 0f;
                    break;
                case "back":
                    //Properties set back
                    gameObject.position.Z = gameObject.previousPosition.Z;
                    gameObject.velocity.Z = 0f;
                    break;
                case "top":
                    //Properties set top
                    gameObject.position.Y = gameObject.previousPosition.Y;
                    gameObject.velocity.Y = 0f;
                    break;
                case "bottom":
                    //Properties set bottom
                    gameObject.position.Y = gameObject.previousPosition.Y;
                    gameObject.velocity.Y = 0f;
                    break;
                default:
                    break;
            }
        }
        #endregion

        //Other
        #region//return an object bij it's name
        /*if (gameObject.name != list[i].name)
                {
                    if (VectorInBox(gameObject.position, list[i].collisionBox, true))
                    {
                        gameObject.position = gameObject.previousPosition;
                        gameObject.velocity = gameObject.previousVelocity;
                    }
                }*/
        #endregion

        //Public methods used to detect partial collisions
        #region//Check if a Vector is top of a box
        public bool VectorTopBox(Vector3 vector, CollisionBox box, bool equalSides, bool equalTopside)
        {
            bool hit = false;
            if (isBetweenLength(vector, box.min, box.max, equalSides))
            {
                if (isBetweenWidth(vector, box.min, box.max, equalSides))
                {
                    if (equalTopside)
                    {
                        if (vector.Y >= box.max.Y)
                        {
                            hit = true;
                        }
                    }
                    else
                    {
                        if (vector.Y > box.max.Y)
                        {
                            hit = true;
                        }
                    }
                }
            }
            return hit;
        }
        #endregion
        #region//Check if a Vector is below a box
        public bool VectorUnderBox(Vector3 vector, CollisionBox box, bool equalSides, bool equalBelowside)
        {
            bool hit = false;
            if (isBetweenLength(vector, box.min, box.max, equalSides))
            {
                if (isBetweenWidth(vector, box.min, box.max, equalSides))
                {
                    if (equalBelowside)
                    {
                        if (vector.Y <= box.min.Y)
                        {
                            hit = true;
                        }
                    }
                    else
                    {
                        if (vector.Y < box.min.Y)
                        {
                            hit = true;
                        }
                    }
                }
            }
            return hit;
        }
        #endregion
        #region//Check if a Vector is right a box
        public bool VectorRightBox(Vector3 vector, CollisionBox box, bool equalSides, bool equalRightside)
        {
            bool hit = false;
            if (isBetweenHeight(vector, box.min, box.max, equalSides))
            {
                if (isBetweenLength(vector, box.min, box.max, equalSides))
                {
                    if (equalRightside)
                    {
                        if (vector.Z >= box.max.Z)
                        {
                            hit = true;
                        }
                    }
                    else
                    {
                        if (vector.Z > box.max.Z)
                        {
                            hit = true;
                        }
                    }
                }
            }
            return hit;
        }
        #endregion
        #region//Check if a Vector is left a box
        public bool VectorLeftBox(Vector3 vector, CollisionBox box, bool equalSides, bool equalLeftside)
        {
            bool hit = false;
            if (isBetweenHeight(vector, box.min, box.max, equalSides))
            {
                if (isBetweenLength(vector, box.min, box.max, equalSides))
                {
                    if (equalLeftside)
                    {
                        if (vector.Z >= box.min.Z)
                        {
                            hit = true;
                        }
                    }
                    else
                    {
                        if (vector.Z > box.min.Z)
                        {
                            hit = true;

                        }
                    }
                }
            }
            return hit;
        }
        #endregion
        #region//Check if a Vector is back of a box
        public bool VectorBackBox(Vector3 vector, CollisionBox box, bool equalSides, bool equalTopside)
        {
            bool hit = false;
            if (isBetweenHeight(vector, box.min, box.max, equalSides))
            {
                if (isBetweenWidth(vector, box.min, box.max, equalSides))
                {
                    if (equalTopside)
                    {
                        if (vector.X <= box.max.X)
                        {
                            hit = true;
                        }
                    }
                    else
                    {
                        if (vector.X < box.max.X)
                        {
                            hit = true;
                        }
                    }
                }
            }
            return hit;
        }
        #endregion
        #region//Check if a Vector is front a box
        public bool VectorFrontBox(Vector3 vector, CollisionBox box, bool equalSides, bool equalBelowside)
        {
            bool hit = false;
            if (isBetweenHeight(vector, box.min, box.max, equalSides))
            {
                if (isBetweenWidth(vector, box.min, box.max, equalSides))
                {
                    if (equalBelowside)
                    {
                        if (vector.X >= box.min.X)
                        {
                            hit = true;
                        }
                    }
                    else
                    {
                        if (vector.X > box.min.X)
                        {
                            hit = true;
                        }
                    }
                }
            }
            return hit;
        }
        #endregion

        //Private methods used to for the collision detection
        #region//Check if a float value is bitween and/or on the box's min.X and max.X
        private bool isBetweenWidth(Vector3 value, Vector3 min, Vector3 max, bool equalSides)
        {
            if (equalSides)
            {
                if ((value.X >= min.X) && (value.X <= max.X))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if ((value.X > min.X) && (value.X < max.X))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        #endregion
        #region//Check if a float value is bitween and/or on the box's min.Y and max.Y
        private bool isBetweenHeight(Vector3 value, Vector3 min, Vector3 max, bool equalSides)
        {
            if (equalSides)
            {
                if ((value.Y >= min.Y) && (value.Y <= max.Y))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if ((value.Y > min.Y) && (value.Y < max.Y))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        #endregion
        #region//Check if a float value is bitween and/or on the box's min.Z and max.Z
        private bool isBetweenLength(Vector3 value, Vector3 min, Vector3 max, bool equalSides)
        {
            if (equalSides)
            {
                if ((value.Z >= min.Z) && (value.X <= max.Z))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if ((value.Z > min.Z) && (value.X < max.Z))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        #endregion

        //Used to create a List containing all corners of a CollisionBox.
        #region//Create CollisionBox corners List
        private List<Vector3> CreateBoxVectList(CollisionBox box)
        {
            //Create the List to return
            List<Vector3> returnList = new List<Vector3>();

            //Adding all the vextor's which needed to be checked
            returnList.Add(box.min);
            returnList.Add(new Vector3(box.max.X, box.min.Y, box.min.Z));
            returnList.Add(new Vector3(box.min.X, box.max.Y, box.min.Z));
            returnList.Add(new Vector3(box.min.X, box.min.Y, box.max.Z));
            returnList.Add(box.max);
            returnList.Add(new Vector3(box.min.X, box.max.Y, box.max.Z));
            returnList.Add(new Vector3(box.max.X, box.min.Y, box.max.Z));
            returnList.Add(new Vector3(box.max.X, box.max.Y, box.min.Z));

            //Return
            return returnList;
        }
        #endregion
    }
}