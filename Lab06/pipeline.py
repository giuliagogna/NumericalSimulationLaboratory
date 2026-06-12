import sys
import os
import subprocess
import shutil
import numpy as np

# This pipeline automatically runs the simulation changing the input.dat file to run at
# the different temperatures.
# It runs the simulation, saves the data and renames the files
# It modifies the input parameters RESTART to 1 and TEMP to the temperature of the last simulation

# Define the base directory for the simulation files
SIMULATOR_DIR = 'NSL_SIMULATOR'

# =================================================================
# SIMULATION PARAMETERS 
# =================================================================
# Equilibration (Hollow run): e.g., 1 block of 1000 steps
EQ_NBLOCKS   = 1       
EQ_NSTEPS    = 1000    

# Measurement
MEAS_NBLOCKS = 100      
MEAS_NSTEPS  = 20000   
# =================================================================

def modify_input(restart_val, temp_val, nblocks, nsteps):
    """Modifies RESTART, TEMP, NBLOCKS, and NSTEPS in the input.dat file."""
    input_path = os.path.join(SIMULATOR_DIR, 'INPUT', 'input.dat')
    try:
        with open(input_path, 'r') as f:
            lines = f.readlines()

        with open(input_path, 'w') as f:
            for line in lines:
                stripped = line.strip().upper()
                if stripped.startswith('RESTART'):
                    f.write(f"RESTART                {restart_val}\n")
                elif stripped.startswith('TEMP'):
                    f.write(f"TEMP                   {temp_val}\n")
                elif stripped.startswith('NBLOCKS'):
                    f.write(f"NBLOCKS                {nblocks}\n")
                elif stripped.startswith('NSTEPS'):
                    f.write(f"NSTEPS                 {nsteps}\n")
                else:
                    f.write(line)
    except FileNotFoundError:
        print(f"Error: {input_path} not found.")
        sys.exit(1)

def run_simulator():
    """Runs the simulator executable."""
    source_dir = os.path.join(SIMULATOR_DIR, 'SOURCE')
    simulator_path = os.path.join(source_dir, 'simulator.exe')
    
    if not os.path.exists(simulator_path) and os.path.exists(os.path.join(source_dir, 'simulator')):
        simulator_path = os.path.join(source_dir, 'simulator')
        
    if not os.path.exists(simulator_path):
        print(f"Error: The simulator at {simulator_path} does not exist.")
        print("Please compile the C++ code first.")
        sys.exit(1)
    
    result = subprocess.run([f'./{os.path.basename(simulator_path)}'], cwd=source_dir, capture_output=True, text=True)
    if result.returncode != 0:
        print("Error running the simulator:")
        print(result.stderr)
        sys.exit(1)

def copy_config():
    """Copies the final spin configuration to the input folder."""
    src = os.path.join(SIMULATOR_DIR, 'OUTPUT', 'CONFIG', 'config.spin')
    dst = os.path.join(SIMULATOR_DIR, 'INPUT', 'CONFIG', 'config.spin')
    
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    
    try:
        shutil.copy(src, dst)
    except FileNotFoundError:
        print(f"Error: Source file {src} not found. Cannot continue.")
        sys.exit(1)

def rename_output_files(temp_val, mode="whole"):
    """Renames output files based on mode, shrinks data, and drops unnecessary logs."""
    output_dir = os.path.join(SIMULATOR_DIR, 'OUTPUT')
    
    # Define temperatures for which we want to keep the full history
    full_history_temps = [2.0, 1.0, 0.5]
    keep_full_history = round(temp_val, 1) in full_history_temps
    
    # Base files to process based on mode
    if mode == "whole":
        files_to_rename = ['total_energy.dat', 'specific_heat.dat', 'susceptibility.dat']
    elif mode == "magnetization":
        files_to_rename = ['magnetization.dat']
    else:
        files_to_rename = []
        
    # We only care about acceptance and output logs for the main temperatures
    if keep_full_history:
        files_to_rename.extend(['acceptance.dat', 'output.dat'])
    
    try:
        temp_str = f"{temp_val:.2f}".replace('.', '_')
        
        for filename in files_to_rename:
            src_path = os.path.join(output_dir, filename)
            if os.path.exists(src_path):
                
                # --- SHRINKING ---
                if not keep_full_history and filename not in ['output.dat', 'acceptance.dat']:
                    with open(src_path, 'r') as f:
                        lines = f.readlines()
                    
                    if len(lines) > 1:
                        shrunk_lines = [lines[0], lines[-1]]
                        with open(src_path, 'w') as f:
                            f.writelines(shrunk_lines)
                # ---------------------------

                name, ext = os.path.splitext(filename)
                new_name = f"{name}_{temp_str}{ext}"
                dst_path = os.path.join(output_dir, new_name)
                os.rename(src_path, dst_path)
                
        # --- CLEANUP ---
        # If we didn't rename acceptance.dat and output.dat, delete them to keep the folder clean
        if not keep_full_history:
            for temp_file in ['acceptance.dat', 'output.dat']:
                temp_path = os.path.join(output_dir, temp_file)
                if os.path.exists(temp_path):
                    os.remove(temp_path)
                    
    except Exception as e:
        print(f"An error occurred during file renaming: {e}")

def main():
    """Main function to control the simulation flow."""
    if len(sys.argv) != 2 or sys.argv[1] not in ['whole', 'magnetization']:
        print("Usage: python pipeline.py [whole|magnetization]")
        sys.exit(1)

    mode = sys.argv[1]

    if mode == "whole":
        answer = input("WARNING: This will start a fresh simulated cooling cycle from T=2.0, overwriting existing files. Proceed? (y/n): ").lower()
        if answer != 'y':
            print("Aborting execution.")
            sys.exit(0)

        temp_start = 2.0
        print(f"\n=== Processing T = {temp_start} ===")
        
        # --- EQUILIBRATION ---
        print("  -> Equilibration phase ...")
        modify_input(restart_val=0, temp_val=temp_start, nblocks=EQ_NBLOCKS, nsteps=EQ_NSTEPS)
        run_simulator()
        
        # --- MEASUREMENT ---
        print("  -> Measurement phase ...")
        copy_config() 
        modify_input(restart_val=1, temp_val=temp_start, nblocks=MEAS_NBLOCKS, nsteps=MEAS_NSTEPS)
        run_simulator()
        rename_output_files(temp_start, mode)

        # DECREASING TEMPERATURE LOOP
        temperatures = np.arange(1.9, 0.45, -0.1) 
        for temp in temperatures:
            current_temp = round(temp, 1)
            print(f"\n=== Processing T = {current_temp} ===")
            
            # --- EQUILIBRATION ---
            print("  -> Equilibration phase ...")
            copy_config() 
            modify_input(restart_val=1, temp_val=current_temp, nblocks=EQ_NBLOCKS, nsteps=EQ_NSTEPS)
            run_simulator()
            
            # --- MEASUREMENT ---
            print("  -> Measurement phase...")
            copy_config() 
            modify_input(restart_val=1, temp_val=current_temp, nblocks=MEAS_NBLOCKS, nsteps=MEAS_NSTEPS)
            run_simulator()
            rename_output_files(current_temp, mode)
            
        print("\nAll 'whole' simulations finished.")

    elif mode == "magnetization":
        print("\n*** WARNING: Remember to manually set h=0.02 in INPUT/input.dat before proceeding! ***")
        answer = input("This will start a fresh MAGNETIZATION cycle from T=2.0, overwriting existing files. Proceed? (y/n): ").lower()
        if answer != 'y':
            print("Aborting execution.")
            sys.exit(0)

        temp_start = 2.0
        print(f"\n=== Processing T = {temp_start} ===")
        
        # --- EQUILIBRATION ---
        print("  -> Equilibration phase ...")
        modify_input(restart_val=0, temp_val=temp_start, nblocks=EQ_NBLOCKS, nsteps=EQ_NSTEPS)
        run_simulator()
        
        # --- MEASUREMENT ---
        print("  -> Measurement phase...")
        copy_config()
        modify_input(restart_val=1, temp_val=temp_start, nblocks=MEAS_NBLOCKS, nsteps=MEAS_NSTEPS)
        run_simulator()
        rename_output_files(temp_start, mode)

        # DECREASING TEMPERATURE LOOP
        temperatures = np.arange(1.9, 0.45, -0.1) 
        for temp in temperatures:
            current_temp = round(temp, 1)
            print(f"\n=== Processing T = {current_temp} ===")
            
            # --- EQUILIBRATION ---
            print("  -> Equilibration phase ...")
            copy_config()
            modify_input(restart_val=1, temp_val=current_temp, nblocks=EQ_NBLOCKS, nsteps=EQ_NSTEPS)
            run_simulator()
            
            # --- MEASUREMENT ---
            print("  -> Measurement phase...")
            copy_config()
            modify_input(restart_val=1, temp_val=current_temp, nblocks=MEAS_NBLOCKS, nsteps=MEAS_NSTEPS)
            run_simulator()
            rename_output_files(current_temp, mode)
            
        print("\nAll 'magnetization' simulations finished.")

if __name__ == "__main__":
    main()