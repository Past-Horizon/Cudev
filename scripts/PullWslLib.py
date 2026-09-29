import shlex
import shutil
import subprocess
import sys
from pathlib import Path


def run_wsl(command):
    result = subprocess.run(
        ["wsl.exe", "--", "bash", "-lc", command],
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(
            result.stderr.strip() or result.stdout.strip() or command
        )
    return result.stdout.strip()


def resolve_wsl_source():
    home = run_wsl('printf "%s\\n" "$HOME"').strip()
    source = f"{home}/.vs/Cudev/CudevBuild/Linux"
    run_wsl(f"test -d {shlex.quote(source)}")
    return source


def wsl_to_windows_path(wsl_path):
    return run_wsl(f"wslpath -w {shlex.quote(wsl_path)}")


def main():
    repo_root = Path(__file__).resolve().parents[1]
    target = (repo_root / "CudevBuild" / "Linux").resolve()

    try:
        source = resolve_wsl_source()
        source_windows = wsl_to_windows_path(source)
    except (RuntimeError, FileNotFoundError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    if target.exists():
        shutil.rmtree(target)

    try:
        shutil.copytree(source_windows, target)
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
