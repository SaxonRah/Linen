/// v ExamplePlugin.cs
using System;
using FlaxEngine;

namespace LinenFlax
{
    /// <summary>
    /// The sample game plugin.
    /// </summary>
    /// <seealso cref="FlaxEngine.GamePlugin" />
    public class LinenFlax : GamePlugin
    {
        /// <inheritdoc />
        public LinenFlax()
        {
            _description = new PluginDescription
            {
                Name = "LinenFlax",
                Category = "Other",
                Author = "",
                AuthorUrl = null,
                HomepageUrl = null,
                RepositoryUrl = "https://github.com/FlaxEngine/LinenFlax",
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
            Debug.Log("Hello from LinenFlax plugin C# code!");
        }

        /// <inheritdoc />
        public override void Deinitialize()
        {
            Debug.Log("Goodbye from LinenFlax plugin C# code!");
            base.Deinitialize();
        }
    }
}
/// ^ ExamplePlugin.cs