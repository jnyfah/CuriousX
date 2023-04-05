class Salary:
    def __init__(self):
        # Initialize instance variables
        self.hourly_rate = 0
        self.total_hours = 0
        self.overtime_pay = 0
        self.total_salary = 0

    def compute_Salary(self, hourly_rate, hours_worked):
        # Compute the total hours worked in the month
        self.hourly_rate = hourly_rate
        self.total_hours = sum(hours_worked)

        # Check if the total hours worked is greater than 140 (4 weeks * 35 hours per week)
        if self.total_hours > 140:
            # Calculate overtime pay for any hours worked above 140
            overtime_hours = self.total_hours - 140
            self.overtime_pay = overtime_hours * self.hourly_rate * 2
            # Calculate total salary, including overtime pay
            self.total_salary = (140 * self.hourly_rate) + self.overtime_pay
        else:
            # Calculate total salary without overtime pay
            self.total_salary = self.total_hours * self.hourly_rate

    def Display(self):
        # Print out the monthly salary in a formatted way
        print("Monthly salary: ${:,.2f}".format(self.total_salary))


if __name__ == "__main__":
    # Create an instance of the Salary class
    s = Salary()
    # Prompt the user to input the hourly rate and hours worked for each week
    hourly_rate = float(input("Enter hourly rate: "))
    hours_worked = [float(input("Enter hours worked for week {} : ".format(i+1))) for i in range(4)]
    # Compute the monthly salary and display it
    s.compute_Salary(hourly_rate, hours_worked)
    s.Display()
