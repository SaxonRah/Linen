/// v ExamplePlugin.cs
using System;
using FlaxEngine;

namespace ExamplePlugin
{
    /// <summary>
    /// The sample game plugin.
    /// </summary>
    /// <seealso cref="FlaxEngine.GamePlugin" />
    public class ExamplePlugin : GamePlugin
    {
        /// <inheritdoc />
        public ExamplePlugin()
        {
            _description = new PluginDescription
            {
                Name = "ExamplePlugin",
                Category = "Other",
                Author = "",
                AuthorUrl = null,
                HomepageUrl = null,
                RepositoryUrl = "https://github.com/FlaxEngine/ExamplePlugin",
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
            Debug.Log("Hello from ExamplePlugin C# code!");
        }

        /// <inheritdoc />
        public override void Deinitialize()
        {
            Debug.Log("Goodbye from ExamplePlugin C# code!");
            base.Deinitialize();
        }
    }
}
/// ^ ExamplePlugin.cs