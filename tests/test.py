import subprocess

TTCLI = "./build/ttcli --json "

def run_ttcli(command):
    """Run a ttcli command and return the result."""
    return subprocess.run([TTCLI + command], capture_output=True, text=True, shell=True)

def test_workspace_list(expected_output):
    """Check list of workspaces."""
    result = run_ttcli('workspace list')
    assert expected_output in result.stdout, f"Expected '{expected_output}' in output, but got:\n{result.stdout}"

def test_window_list(expected_output):
    """Check list of windows."""
    result = run_ttcli('window list')
    assert expected_output in result.stdout, f"Expected '{expected_output}' in output, but got:\n{result.stdout}"

def test_workspace_switch(destination_workspace):
    """Check workspace switch."""
    result = run_ttcli(f'workspace switch {destination_workspace}')
    expected_success_message = f'{{"success": "switch to workspace {destination_workspace}"}}'
    assert expected_success_message in result.stdout, f"Expected 'switch to workspace {destination_workspace}' in output, but got:\n{result.stdout}"

if __name__ == '__main__':
    test_workspace_list('[ { "name": "main", "active": true }, { "name": "test", "active": false } ]')
    test_window_list('[ { "title": "simple-egl", "workspace": "main" }, { "title": "simple-damage", "workspace": "main" } ]')
    test_workspace_switch('test')
    test_workspace_list('[ { "name": "main", "active": false }, { "name": "test", "active": true } ]')
    run_ttcli('exit')
