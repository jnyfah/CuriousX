#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_map>

class RegisterAllocator {
public:
    RegisterAllocator() {
        _free_registers.set();
    }

    // Allocate a free register
    int alloc_register() {
        int reg = _free_registers._Find_first();
        if (reg == _free_registers.size()) {
            // All registers are in use, spill one to memory
            reg = spill_register();
        } else {
            _free_registers.reset(reg);
        }
        return reg;
    }

    // Free an allocated register
    void free_register(int reg) {
        if (_spilled_registers.count(reg) > 0) {
            // Register was spilled, reload from memory
            reload_register(reg);
            _spilled_registers.erase(reg);
        } else {
            _free_registers.set(reg);
        }
    }

    // Set all registers as available
    void free_all_registers() {
        _free_registers.set();
        _spilled_registers.clear();
    }

    // Get the value held by a register
    int get_register_value(int reg) {
        if (_spilled_registers.count(reg) > 0) {
            // Register was spilled, get value from memory
            return _memory[_spilled_registers[reg]];
        } else {
            return _registers[reg];
        }
    }

    // Set the value of a register
    void set_register_value(int reg, int value) {
        if (_spilled_registers.count(reg) > 0) {
            // Register was spilled, set value in memory
            _memory[_spilled_registers[reg]] = value;
        } else {
            _registers[reg] = value;
        }
    }

    // Get the index of the last allocated register
    int get_last_allocated_register() const {
        int last_allocated = 0;

        return last_allocated;
    }


private:
    std::bitset<13> _free_registers; // Available registers
    std::unordered_map<int, int> _spilled_registers; // Spilled registers (register -> memory location)
    std::vector<int> _registers; // Register values
    std::vector<int> _memory; // Memory to store spilled registers

// Spill a register to memory and return the memory location
    int spill_register() {
        for (int reg = 0; reg < 13; reg++) {
            if (_spilled_registers.find(reg) == _spilled_registers.end()) {
                // Register is not spilled yet, spill it
                int mem_loc = _memory.size();
                _memory.push_back(_registers[reg]);
                _spilled_registers[reg] = mem_loc;
                _free_registers.reset(reg);
                return reg;
            }
        }
        // All registers are already spilled, spill the first one
        auto it = _spilled_registers.begin();
        int reg = it->first;
        int mem_loc = it->second;
        _spilled_registers.erase(it);
        _memory[mem_loc] = _registers[reg];
        _spilled_registers[reg] = mem_loc;
        return reg;
    }


    // Reload a spilled register from memory
    void reload_register(int reg) {
        int mem_loc = _spilled_registers[reg];
        _registers[reg] = _memory[mem_loc];
        _free_registers.reset(reg);
    }
};
