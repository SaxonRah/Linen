using Flax.Build;

public class LinenEditorTarget : GameProjectEditorTarget
{
    /// <inheritdoc />
    public override void Init()
    {
        base.Init();

        // Reference the modules for editor
        Modules.Add("Linen");
        Modules.Add("LinenEditor");
    }
}
