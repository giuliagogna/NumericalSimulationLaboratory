import sys
import os
import subprocess
import shutil
import numpy as np

# TODO: Implement the autocorrelation calculations

# Define the base directory for the simulation files
SIMULATOR_DIR = 'NSL_SIMULATOR'

def modify_input(restart_val, temp_val):
    """Modifies RESTART and TEMP in the input.dat file."""
    input_path = os.path.join(SIMULATOR_DIR, 'INPUT', 'input.dat')
    try:
        with open(input_path, 'r') as f:
            lines = f.readlines()

        with open(input_path, 'w') as f:
            for line in lines:
                if line.strip().upper().startswith('RESTART'):
                    f.write(f"RESTART {restart_val}\n")
                elif line.strip().upper().startswith('TEMP'):
                    f.write(f"TEMP {temp_val}\n")
                else:
                    f.write(line)
    except FileNotFoundError:
        print(f"Error: {input_path} not found.")
        sys.exit(1)

def run_simulator():
    """Runs the simulator executable."""
    source_dir = os.path.join(SIMULATOR_DIR, 'SOURCE')
    simulator_path = os.path.join(source_dir, 'simulator.exe')
    if not os.path.exists(simulator_path):
        print(f"Error: The simulator at {simulator_path} does not exist.")
        print("Please compile the C++ code first.")
        sys.exit(1)
    
    print("Running the simulation...")
    result = subprocess.run([f'./{os.path.basename(simulator_path)}'], cwd=source_dir, capture_output=True, text=True)
    if result.returncode != 0:
        print("Error running the simulator:")
        print(result.stderr)
        sys.exit(1)
    print("Simulation complete.")

def copy_config():
    """Copies the final spin configuration to the input folder."""
    src = os.path.join(SIMULATOR_DIR, 'OUTPUT', 'CONFIG', 'config.spin')
    dst = os.path.join(SIMULATOR_DIR, 'INPUT', 'CONFIG', 'config.spin')
    
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    
    try:
        shutil.copy(src, dst)
        print(f"Copied {src} to {dst}")
    except FileNotFoundError:
        print(f"Error: Source file {src} not found. Cannot continue.")
        sys.exit(1)

def rename_output_files(temp_val, mode="whole"):
    """Renames output files based on mode (whole or magnetization)."""
    output_dir = os.path.join(SIMULATOR_DIR, 'OUTPUT')
    
    # Files we ALWAYS want to save regardless of the mode
    files_to_rename = ['acceptance.dat', 'output.dat']
    
    # Add mode-specific files
    if mode == "whole":
        files_to_rename += ['total_energy.dat', 'specific_heat.dat', 'susceptibility.dat']
    elif mode == "magnetization":
        files_to_rename += ['magnetization.dat']
    
    try:
        temp_str = f"{temp_val:.2f}".replace('.', '_')
        
        for filename in files_to_rename:
            src_path = os.path.join(output_dir, filename)
            if os.path.exists(src_path):
                name, ext = os.path.splitext(filename)
                new_name = f"{name}_{temp_str}{ext}"
                dst_path = os.path.join(output_dir, new_name)
                os.rename(src_path, dst_path)
                print(f"Renamed {src_path} to {dst_path}")
            else:
                pass # Silent skip for missing files to avoid clutter
    except Exception as e:
        print(f"An error occurred during file renaming: {e}")

def main():
    """Main function to control the simulation flow."""
    if len(sys.argv) != 2 or sys.argv[1] not in ['whole', 'magnetization']:
        print("Usage: python pipeline.py [whole|magnetization]")
        sys.exit(1)

    mode = sys.argv[1]

    if mode == "whole":
        answer = input("Is this the first time you are running the 'whole' simulation? (y/n): ").lower()
        if answer != 'y':
            print("Aborting. Please change the parameters in INPUT/input.dat manually.")
            sys.exit(0)

        temp_start = 2.0
        print(f"\nSetting RESTART to 0 and TEMP to {temp_start} in INPUT/input.dat.")
        modify_input(0, temp_start)
        run_simulator()
        rename_output_files(temp_start, mode)

        temperatures = np.arange(1.9, 0.45, -0.1) 
        for temp in temperatures:
            current_temp = round(temp, 1)
            print("\n" + "="*30)
            print(f"Preparing for simulation at T = {current_temp}")
            
            copy_config()
            
            print(f"Setting RESTART to 1 and TEMP to {current_temp}.")
            modify_input(1, current_temp)
            
            run_simulator()
            rename_output_files(current_temp, mode)
            
        print("\nAll 'whole' simulations finished.")

    elif mode == "magnetization":
        print("\n*** WARNING: Remember to manually set h=0.02 in INPUT/input.dat before proceeding! ***")
        answer = input("Is this the first time you are running the MAGNETIZATION simulation? (y/n): ").lower()
        if answer != 'y':
            print("Aborting. Please change the parameters in INPUT/input.dat manually.")
            sys.exit(0)

        temp_start = 2.0
        print(f"\nSetting RESTART to 0 and TEMP to {temp_start} in INPUT/input.dat.")
        modify_input(0, temp_start)
        run_simulator()
        rename_output_files(temp_start, mode)

        temperatures = np.arange(1.9, 0.45, -0.1) 
        for temp in temperatures:
            current_temp = round(temp, 1)
            print("\n" + "="*30)
            print(f"Preparing for magnetization simulation at T = {current_temp}")
            
            copy_config()
            
            print(f"Setting RESTART to 1 and TEMP to {current_temp}.")
            modify_input(1, current_temp)
            
            run_simulator()
            rename_output_files(current_temp, mode)
            
        print("\nAll 'magnetization' simulations finished.")

if __name__ == "__main__":
    main()