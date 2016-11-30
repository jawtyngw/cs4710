#region File Description
//-----------------------------------------------------------------------------
// Game1.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#endregion

using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;
using System.IO;

namespace WindowsGame1
{
    /// <summary>
    /// This is the main type for your game
    /// </summary>
    public class Game1 : Microsoft.Xna.Framework.Game
    {
        Matrix worldMatrix;
        Matrix viewMatrix;
        Matrix projectionMatrix;
        VertexDeclaration vertexDeclaration;
        VertexBuffer vertexBuffer;
        BasicEffect basicEffect;
        static float ballScale = 1f / 74.68f;

        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;

        float speed = 0;
        float cameraY = 0;
        float oriCameraOffsetX = 0;
        float oriCameraOffsetY = 0;
        float oriCameraOffsetZ = 0;
        float cameraOffsetX = 0;
        float cameraOffsetY = 0;
        float cameraOffsetZ = 0;
        double cameraAngle = 0;
        float cameraYInterval = 0;
        float oriCameraRotationRadius = 0;
        float cameraRotationRadius = 0;
        Vector3 centerPosition;

        List<Model> models = new List<Model>();

        // Set the position of the model in world space, and set the rotation.
        List<Vector3> modelPositions = new List<Vector3>();
        float modelRotation = 0.0f;


        public Game1()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
        }

        private void CreateVertexBuffer()
        {
            vertexDeclaration = new VertexDeclaration(new VertexElement[]
                {
                    new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position, 0),
                    new VertexElement(12, VertexElementFormat.Vector3, VertexElementUsage.Normal, 0),
                    new VertexElement(24, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0)
                }
            );

            vertexBuffer = new VertexBuffer(
                graphics.GraphicsDevice,
                vertexDeclaration,
                1,
                BufferUsage.None
                );

            graphics.GraphicsDevice.SetVertexBuffer(vertexBuffer);
        }


        protected override void Initialize()
        {
            // TODO: Add your initialization logic here
            CreateVertexBuffer();

            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            InitializeTransform();
            InitializeEffect();
            spriteBatch = new SpriteBatch(GraphicsDevice);
            /* raw data
            string text = File.ReadAllText(@"data.txt");
            foreach(string line in text.Split('\n'))
            {
                string[] split = line.Split(' ');
                if (split.Count() < 3)
                {
                    speed = split[0];
                    break;
                }
                float y = -float.Parse(split[0]) / 1000, x = float.Parse(split[1]) / 1000, z = float.Parse(split[2]) / 1000;
                models.Add(Content.Load<Model>("Pokeball"));
                modelPositions.Add(new Vector3(x, y, z));
            }*/


            /* function */
            try
            {
                string text = File.ReadAllText(@"C:\General use\Homework\CS 4710\data\func.txt");
                string[] lines = text.Split('\n');
                float xa = float.Parse(lines[0]), xb = float.Parse(lines[1]), xc = float.Parse(lines[2]);
                float ya = float.Parse(lines[3]), yb = float.Parse(lines[4]);
                float za = float.Parse(lines[5]), zb = float.Parse(lines[6]), zc = float.Parse(lines[7]);
                float t = float.Parse(lines[8]);
                speed = float.Parse(lines[9]);
                for (float i = 0; i < t; i += 0.005f)
                {
                    float y = -(xa * i * i + xb * i + xc) / 1000;
                    float x = (ya + yb * i) / 1000;
                    float z = (za + zb * (float)Math.Log(i + zc)) / 1000;
                    models.Add(Content.Load<Model>("Pokeball"));
                    modelPositions.Add(new Vector3(x, y, z));
                }

                float ax = (modelPositions[0].X + modelPositions.Last().X) / 2;
                float ay = (modelPositions[0].Y + modelPositions.Last().Y) / 2;
                float az = (modelPositions[0].Z + modelPositions.Last().Z) / 2;
                centerPosition = new Vector3(ax, ay, az);

                oriCameraOffsetX = ax;
                oriCameraOffsetY = ay;
                oriCameraOffsetZ = az - modelPositions[0].Z;
                cameraOffsetX = oriCameraOffsetX;
                cameraOffsetY = oriCameraOffsetY;
                cameraOffsetZ = oriCameraOffsetZ;
                oriCameraRotationRadius = (az - cameraOffsetZ) * 2;
                cameraRotationRadius = oriCameraRotationRadius;
                cameraYInterval = cameraRotationRadius / 20;

            }
            catch (Exception) { }
        }

        /// <summary>
        /// Initializes the transforms used for the 3D model.
        /// </summary>
        private void InitializeTransform()
        {
            float tilt = MathHelper.ToRadians(0);  // 0 degree angle
            // Use the world matrix to tilt the cube along x and y axes.
            worldMatrix = Matrix.CreateRotationX(tilt) * Matrix.CreateRotationY(tilt);
            viewMatrix = Matrix.CreateLookAt(new Vector3(5, 5, 5), Vector3.Zero, Vector3.Up);

            projectionMatrix = Matrix.CreatePerspectiveFieldOfView(
                MathHelper.ToRadians(45),  // 45 degree angle
                (float)GraphicsDevice.Viewport.Width /
                (float)GraphicsDevice.Viewport.Height,
                1.0f, 100.0f);
        }

        /// <summary>
        /// Initializes the basic effect (parameter setting and technique selection)
        /// used for the 3D model.
        /// </summary>
        private void InitializeEffect()
        {
            basicEffect = new BasicEffect(graphics.GraphicsDevice);

            basicEffect.World = worldMatrix;
            basicEffect.View = viewMatrix;
            basicEffect.Projection = projectionMatrix;

            // primitive color
            basicEffect.AmbientLightColor = new Vector3(0.1f, 0.1f, 0.1f);
            basicEffect.DiffuseColor = new Vector3(1.0f, 1.0f, 1.0f);
            basicEffect.SpecularColor = new Vector3(0.25f, 0.25f, 0.25f);
            basicEffect.SpecularPower = 5.0f;
            basicEffect.Alpha = 1.0f;

            basicEffect.LightingEnabled = true;
            if (basicEffect.LightingEnabled)
            {
                basicEffect.DirectionalLight0.Enabled = true; // enable each light individually
                if (basicEffect.DirectionalLight0.Enabled)
                {
                    // x direction
                    basicEffect.DirectionalLight0.DiffuseColor = new Vector3(1, 0, 0); // range is 0 to 1
                    basicEffect.DirectionalLight0.Direction = Vector3.Normalize(new Vector3(-1, 0, 0));
                    // points from the light to the origin of the scene
                    basicEffect.DirectionalLight0.SpecularColor = Vector3.One;
                }

                basicEffect.DirectionalLight1.Enabled = true;
                if (basicEffect.DirectionalLight1.Enabled)
                {
                    // y direction
                    basicEffect.DirectionalLight1.DiffuseColor = new Vector3(0, 0.75f, 0);
                    basicEffect.DirectionalLight1.Direction = Vector3.Normalize(new Vector3(0, -1, 0));
                    basicEffect.DirectionalLight1.SpecularColor = Vector3.One;
                }

                basicEffect.DirectionalLight2.Enabled = true;
                if (basicEffect.DirectionalLight2.Enabled)
                {
                    // z direction
                    basicEffect.DirectionalLight2.DiffuseColor = new Vector3(0, 0, 0.5f);
                    basicEffect.DirectionalLight2.Direction = Vector3.Normalize(new Vector3(0, 0, -1));
                    basicEffect.DirectionalLight2.SpecularColor = Vector3.One;
                }
            }

        }
        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            // Allows the game to exit
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
                this.Exit();

            // TODO: Add your update logic here
#if WINDOWS
            if (Keyboard.GetState().IsKeyDown(Keys.Escape))
                this.Exit();
            if (Keyboard.GetState().IsKeyDown(Keys.Left))
                cameraAngle -= Math.PI / 100;
            if (Keyboard.GetState().IsKeyDown(Keys.Right))
                cameraAngle += Math.PI / 100;
            if (Keyboard.GetState().IsKeyDown(Keys.Up))
                cameraY += cameraYInterval;
            if (Keyboard.GetState().IsKeyDown(Keys.Down))
                cameraY -= cameraYInterval;
            if (Keyboard.GetState().IsKeyDown(Keys.Q) && cameraRotationRadius <= oriCameraRotationRadius * 10)
                cameraRotationRadius += oriCameraRotationRadius / 50;
            if (Keyboard.GetState().IsKeyDown(Keys.A) && cameraRotationRadius >= oriCameraRotationRadius / 10)
                cameraRotationRadius -= oriCameraRotationRadius / 50;
#endif


            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            graphics.GraphicsDevice.Clear(Color.SteelBlue);
                        
            spriteBatch.Begin();

            // this being the line that answers your question
            spriteBatch.DrawString(Content.Load<SpriteFont>("MyFont"), "Speed: " + speed, new Vector2(10, 10), Color.White);

            spriteBatch.End();

            RasterizerState rasterizerState1 = new RasterizerState();
            rasterizerState1.CullMode = CullMode.None;
            graphics.GraphicsDevice.RasterizerState = rasterizerState1;
            /*foreach (EffectPass pass in basicEffect.CurrentTechnique.Passes)
            {
                pass.Apply();

                graphics.GraphicsDevice.DrawPrimitives(
                    PrimitiveType.TriangleList,
                    0,
                    12
                );
            }*/
            Vector3 cameraPosition =
                new Vector3((float)(cameraRotationRadius * Math.Cos(cameraAngle)) + centerPosition.X,
                            cameraY + cameraOffsetY,
                            (float)(cameraRotationRadius * Math.Sin(cameraAngle)) + centerPosition.Z);
            Matrix view = Matrix.CreateLookAt(cameraPosition, centerPosition, Vector3.Up);
            for (int i = 0; i < models.Count; i++)
            {
                Model model = models[i];
                Matrix[] transforms = new Matrix[model.Bones.Count];
                model.CopyAbsoluteBoneTransformsTo(transforms);

                // Draw the model. A model can have multiple meshes, so loop.
                foreach (ModelMesh mesh in model.Meshes)
                {
                    // This is where the mesh orientation is set, as well 
                    // as our camera and projection.
                    foreach (BasicEffect effect in mesh.Effects)
                    {
                        effect.EnableDefaultLighting();
                        effect.World = transforms[mesh.ParentBone.Index] * 
                            Matrix.CreateScale(ballScale, ballScale, ballScale) *
                            Matrix.CreateRotationY(modelRotation) *
                            Matrix.CreateTranslation(modelPositions[i]);
                        effect.View = view;
                        effect.Projection = Matrix.CreatePerspectiveFieldOfView(
                            MathHelper.ToRadians(45.0f), graphics.GraphicsDevice.Viewport.AspectRatio,
                            1.0f, 10000.0f);
                    }
                    // Draw the mesh, using the effects set above.
                    mesh.Draw();
                }
            }

            base.Draw(gameTime);
        }
    }
}
