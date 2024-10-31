import subprocess
import json

TTCLI = "./build/ttcli --json "

def run_ttcli(command):
    """Run a ttcli command and return the result."""
    return subprocess.run([TTCLI + command], capture_output=True, text=True, shell=True)

def test_workspace_list(expected_output):
    """Check list of workspaces, ignoring irrelevant fields."""
    result = run_ttcli('workspace list')
    workspaces = json.loads(result.stdout)

    # Extract only 'name' and 'active' fields for comparison
    actual_output = [{ "name": w["name"], "active": w["active"] } for w in workspaces]
    assert actual_output == expected_output, f"Expected {expected_output} but got {actual_output}"

def test_window_list(expected_titles):
    """Check list of windows, ignoring IDs."""
    result = run_ttcli('window list')
    windows = json.loads(result.stdout)

    # Extract 'title' and 'workspace' fields only
    actual_titles = [{ "title": w["title"], "workspace": w["workspace"] } for w in windows]
    assert actual_titles == expected_titles, f"Expected {expected_titles} but got {actual_titles}"

def test_workspace_switch(destination_workspace):
    """Check workspace switch."""
    result = run_ttcli(f'workspace switch {destination_workspace}')
    expected_success_message = f'{{"success": "switch to workspace {destination_workspace}"}}'
    assert expected_success_message in result.stdout, f"Expected 'switch to workspace {destination_workspace}' in output, but got:\n{result.stdout}"

if __name__ == '__main__':
    test_workspace_list([
        { "name": "main", "active": True },
        { "name": "test", "active": False }
    ])
    test_window_list([
        { "title": "simple-egl", "workspace": "main" },
        { "title": "simple-damage", "workspace": "main" }
    ])
    test_workspace_switch('test')
    test_workspace_list([
        { "name": "main", "active": False },
        { "name": "test", "active": True }
    ])
    run_ttcli('exit')
