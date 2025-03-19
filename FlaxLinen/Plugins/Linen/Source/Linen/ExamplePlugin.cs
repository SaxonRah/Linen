using System;
using FlaxEngine;

namespace Linen
{
    /// <summary>
    /// The sample game plugin.
    /// </summary>
    /// <seealso cref="FlaxEngine.GamePlugin" />
    public class Linen : GamePlugin
    {
        /// <inheritdoc />
        public Linen()
        {
            _description = new PluginDescription
            {
                Name = "Linen",
                Category = "Other",
                Author = "ParabolicLabs",
                AuthorUrl = null,
                HomepageUrl = null,
                RepositoryUrl = "https://github.com/FlaxEngine/Linen",
                Description = "This is an example plugin project.",
                Version = new Version(0, 0, 1),
                IsAlpha = false,
                IsBeta = false,
            };
        }

        /// <inheritdoc />
        public override void Initialize()
        {
            base.Initialize();

            Debug.Log("Hello from plugin code!");
        }

        /// <inheritdoc />
        public override void Deinitialize()
        {
            // Use it to cleanup data

            base.Deinitialize();
        }
    }
}
