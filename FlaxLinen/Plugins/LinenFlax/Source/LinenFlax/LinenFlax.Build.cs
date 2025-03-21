// v LinenFlax.Build.cs
using Flax.Build;
using Flax.Build.NativeCpp;

public class LinenFlax : GameModule
{
    /// <inheritdoc />
    public override void Setup(BuildOptions options)
    {
        base.Setup(options);
        BuildNativeCode = true;
        options.PublicDefinitions.Add("COMPILE_WITH_FLAX");
    }
}
// ^ LinenFlax.Build.cs