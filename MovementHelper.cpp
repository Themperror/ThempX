												   using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Net;
using Microsoft.Xna.Framework.Storage;
using Game_FMG.Scripts;

namespace Game_FMG.Scripts
{
    class MovementHelper
    {
        private Matrix movementMatrix;
        private Vector3 worldMovement;

        #region//set the velocity to the angle that's given
        public void setAngledVelocity(GameObject gameObject, Vector3 direction, Vector3 movement, float multiplier)
        {
            //Creates a Matrix in which the rotation values are stored (in radians)
            movementMatrix = Matrix.CreateFromYawPitchRoll(direction.Y, direction.X, direction.Z);
            //Calculate the amount of movement in the world
            worldMovement = Vector3.Transform(movement, movementMatrix);

            //adding the distance
            gameObject.velocity = worldMovement * multiplier;
        }
        #endregion
        #region//returns the velocity to the angle that's given
        public Vector3 getAngledVelocity(GameObject gameObject, Vector3 direction, Vector3 movement, float multiplier)
        {
            //Creates a Matrix in which the rotation values are stored (in radians)
            movementMatrix = Matrix.CreateFromYawPitchRoll(direction.Y, direction.X, direction.Z);
            //Calculate the amount of movement in the world
            worldMovement = Vector3.Transform(movement, movementMatrix);

            //adding the distance
            return worldMovement * multiplier;
        }
        #endregion
    }
}