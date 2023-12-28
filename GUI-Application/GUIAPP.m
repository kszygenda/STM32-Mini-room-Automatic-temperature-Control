classdef GUIAPP < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        % GUI variables
        UIFigure      matlab.ui.Figure
        StartButton   matlab.ui.control.Button
        PauseButton   matlab.ui.control.Button
        ResumeButton  matlab.ui.control.Button
        SaveButton    matlab.ui.control.Button
        SetButton      matlab.ui.control.Button
        SimulateButton matlab.ui.control.Button
        SetTempSlider matlab.ui.control.Slider
        CurrentTempLabel matlab.ui.control.Label
        CurrentTemp_data matlab.ui.control.Label
        SetTempLabel  matlab.ui.control.Label
        TempTrendAxes matlab.ui.control.UIAxes
        SetTempEditField matlab.ui.control.EditField
        ConnectButton matlab.ui.control.Button
        SerialConnection 
        % Function variables
        MyVector = [];
        t;
        isRunning = 0;
        t_plot = [0];
        start_time;
        stop_time;
        lgd;
        plotHandles;
        
    end
        
    

    % Callbacks that handle component events
    methods (Access = private)
        %% startup/close functions
        % Code that executes after component creation
        function startupFcn(app)
            % Code to initialize app, welcome the user.
            username = getenv('USERNAME');
            disp("Hello " + username + "!");
        
            % left yyaxis 
            yyaxis(app.TempTrendAxes, 'left');
            app.plotHandles(1:3) = plot(app.TempTrendAxes, app.t_plot, nan(1,3));
            ylim(app.TempTrendAxes, [0 100]);
            
            % right y-axis for pwm 
            yyaxis(app.TempTrendAxes, 'right');
            app.plotHandles(4) = plot(app.TempTrendAxes, app.t_plot, nan);
            
            % legenda
            legend(app.TempTrendAxes, {'Temperature', 'Error', 'T_set', 'PWM'}, 'Location', 'northwest');
        end
        function closeRequestFcn(app, event)
            % Code that executes when the app is closed
            % TODO - debugging, potential memory leak(?)
            clear variables;
            delete(app);
        end
        %% User defined function section
        %Function to collect sample from temperature sensor
        function sample_val = collect_sample(app)
            % Code to collect samples from temperature sensor
            % Implementacja funkcji zbierającej z portu szeregowego
                str_received_data = fgetl(app.SerialConnection);
                disp(str_received_data)
                % jeżeli odebrano dane, zdekoduj je z json i zapisz, jeżeli nie to 0.
                % Obsługa 0 jest później.
                %EN: if data received, decode it from json and save, if not, save 0.
                %0 handling is done later.
                if ~isempty(str_received_data)
                    sample_val = jsondecode(str_received_data);
                else
                    sample_val = [];
                end
        end
        % Button pushed function for StartButton
        function StartButtonPushed(app, event)
            %Flaga isrunning do obługi logiki przycisków,
            %EN: isrunning flag to handle buttons logic
            if app.isRunning == 1
                app.isRunning = 0;
                disp('Stopping...')
                stop(app.t); 
                delete(app.t);
            end
            app.isRunning = 1;
            app.MyVector=[];
            app.start_time=[];
            app.stop_time=[];
            app.t=[];
            app.start_time=tic;
            disp('Starting...')
            % Timer użyty jest do cyklicznego wykonywania funkcji
            % plotującej, dodającej wartości do rysowanej charakterystyki.
            % Period powinien być mniejszy od timera STM32. 
            %EN: Timer is used to cyclically execute plotting function,
            %adding values to drawn characteristic. Period SHOULD be
            %lower than STM32 timer.

            %flush czyści bufor portu szeregowego, aby nie było w nim
            %starych danych. Debug restartu zbierania danych
            %EN: flush clears serial port buffer, so there are no old data.
            %Debug of data collection restart.
            flushinput(app.SerialConnection);
            flushoutput(app.SerialConnection);
            app.t = timer('ExecutionMode', 'fixedRate', 'Period', 0.01, 'TimerFcn', @(~,~) cyclic_function(app));
            start(app.t);
        end

        % Funkcja cykliczna 
        function cyclic_function(app)
            % Code to collect and plot data
            samples = collect_sample(app);
            % Jeżeli odebrano 0 (brak danych),powtórz ostatnio odebraną wartość
            % If no data received, repeat last received value
            if isempty(samples)
                data = app.MyVector(end,:); 
            else
                data = [samples.temperature samples.error samples.pwm_power samples.destined];
                disp(data)
            end
            % Rozszerzenie wektora o odebraną wartość
            % EN: Extending vector by received value
            app.MyVector = [app.MyVector;data];
            % Rysowanie i Aktualizacja wykresu
            % EN: Drawing and updating plot
            % Odebranie wartości 
            app.CurrentTemp_data.Text = num2str(app.MyVector(end,1));
            % Odebranie czasu 
            app.stop_time=toc(app.start_time);  
            % Ustalenie nowego Xlim na podstawie nowego czasu oraz nowy
            % wektor czasu do plotowania.
            app.TempTrendAxes.XLim = [0 app.stop_time];
            
            % Lewa strona wykresu destined, current i error
            for i = 1:4
                app.plotHandles(i).XData = app.t_plot;
                app.plotHandles(i).YData = app.MyVector(:,i);
            end
            app.TempTrendAxes.YLim = [0 max([max(app.MyVector(:,1)) max(app.MyVector(:,4))])];
            % Prawa strona wykresu, PWM POWER
            drawnow;
            app.t_plot = [app.t_plot, app.stop_time];
        end

        % Button pushed function for PauseButton
        function PauseButtonPushed(app, event)
            if app.isRunning == 1
                app.isRunning = 0;
                disp('Paused')
                stop(app.t); 
            end
        end

        % Button pushed function for ResumeButton
        function ResumeButtonPushed(app, event)
            if app.isRunning == 0
                app.isRunning = 1;
                disp('Resumed')
                % użyto flush bo zatrzymanie timera nie czyści bufora, dalej są tam wysyłane dane
                %EN: flush used because timer stop doesn't clear buffer, data is still sent there
                flushinput(app.SerialConnection);
                flushoutput(app.SerialConnection);
                start(app.t);
            end
        end

        % Button pushed function for SaveButton
        function SaveButtonPushed(app, event)
            % Define file types for saving
            filter = {'*.csv', 'CSV file (*.csv)'; ...
                      '*.png', 'PNG image (*.png)'; ...
                      '*.jpg', 'JPEG image (*.jpg)'; ...
                      '*.pdf', 'PDF file (*.pdf)'};

            % Open save dialog
            [file, path, indx] = uiputfile(filter, 'Save as');

            % Check if user selected a file
            if isequal(file, 0) || isequal(path, 0)
                uialert(app.UIFigure, 'File save cancelled', 'Info');
            else
                % Full path of the file
                filename = fullfile(path, file);

            % Save based on selected file type
                switch indx
                    case 1 % CSV
                        % Create a table with time and data
                        T = table(app.t_plot', app.MyVector', 'VariableNames', {'Time', 'Value'});
                        % Write table to CSV
                        writetable(T, filename);
                    case 2 % PNG
                        exportgraphics(app.TempTrendAxes, filename, 'Resolution', 300);
                    case 3 % JPG
                        exportgraphics(app.TempTrendAxes, filename, 'Resolution', 300);
                    case 4 % PDF
                        exportgraphics(app.TempTrendAxes, filename, 'ContentType', 'vector');
                    otherwise
                        uialert(app.UIFigure, 'Unknown file type selected', 'Error');
                end
            end
        end
        

        % Value changing function for Slider
        function SetTempSliderValueChanged(app, event)
            value = app.SetTempSlider.Value;
            % Code to update temperature setting
            app.SetTempEditField.Value = num2str(value); % Update edit field with slider value
        end
        
        % Value changing function for Edit Field
        function SetTempEditFieldValueChanged(app, event)
            value = str2double(app.SetTempEditField.Value);
            % Code to update temperature setting
            app.SetTempSlider.Value = value; % Update slider with edit field value
        end

        % Button pushed function for SetButton
        function SetButtonPushed(app, event)
            % Pobierz wartość temperatury i konwertuj na tekst
            currentTempValue = app.SetTempEditField.Value;
            tempValueStr = num2str(str2double(currentTempValue), '%.4f'); % Konwersja z zachowaniem 4 miejsc po przecinku
        
            % Dodaj znaki końca linii i powrotu karetki do wysyłanej wiadomości
            messageToSend = [tempValueStr '\r\n'];
        
            % Sprawdź, czy połączenie szeregowe jest aktywne
            if ~isempty(app.SerialConnection) && isvalid(app.SerialConnection)
                try
                    % Wyślij dane przez port szeregowy
                    writeline(app.SerialConnection, messageToSend);
                catch e
                    % Wyświetl komunikat o błędzie, jeśli coś pójdzie nie tak
                    uialert(app.UIFigure, ['Error sending data: ' e.message], 'Send Error');
                end
            else
                % Wyświetl komunikat, jeśli połączenie szeregowe nie jest aktywne
                uialert(app.UIFigure, 'Serial connection is not established.', 'Connection Error');
            end
        end


        % Button pushed function for SimulateButton
        % TODO: Estimate transfer function parameters,
        function SimulateButtonPushed(app, event)
            % Placeholder for Simulate button functionality
        end

        % Callback function for ConnectButton
        function ConnectButtonPushed(app, event)
            % Get screen size
            screenSize = get(groot, 'ScreenSize');
            screenWidth = screenSize(3);
            screenHeight = screenSize(4);
        
            % Set the size of the UIFigure
            figureWidth = 300;
            figureHeight = 200;

             % Calculate the position to center the figure
            positionX = (screenWidth - figureWidth) / 2;
            positionY = (screenHeight - figureHeight) / 2;

            % Create a new figure for the connection dialog
            dialog = uifigure('Name', 'Serial Port Connection', 'Position', [positionX, positionY, figureWidth, figureHeight]);
        
            % Create a listbox for COM port selection
            comListBox = uilistbox(dialog, 'Position', [20 80 260 70]);
            comListBox.Items = {'COM1', 'COM2', 'COM3', 'COM4', 'COM5', 'COM6', 'COM7', 'COM8'}; % TODO: Replace with actual COM ports list
        
            % Create Connect and Disconnect buttons
            connectButton = uibutton(dialog, 'push', 'Text', 'Connect', 'Position', [50, 20, 80, 30]);
            disconnectButton = uibutton(dialog, 'push', 'Text', 'Disconnect', 'Position', [170, 20, 80, 30]);
        
            % Callbacks for the buttons
            connectButton.ButtonPushedFcn = @(src, event) ConnectToSerialPort(app, comListBox.Value);
            disconnectButton.ButtonPushedFcn = @(src, event) DisconnectSerialPort(app);
        end

        % Placeholder function for ConnectToSerialPort
        function ConnectToSerialPort(app, comPort)
            try
                % Display waitbar during connection process
                hWaitbar = waitbar(0, 'Connecting...', 'Name', 'Connecting to Serial Port', 'CreateCancelBtn', 'delete(gcbf)');
                % Create and open serial port connection
                app.SerialConnection = serialport(comPort, 115200, 'DataBits', 8, 'Parity', 'None', 'StopBits', 1, 'FlowControl', 'none');
                fopen(app.SerialConnection);
                waitbar(1, hWaitbar, 'Connection established');
                pause(1); % Pause to show the waitbar
            catch e
                uialert(app.UIFigure, ['Error connecting to ' comPort ': ' e.message], 'Connection Error');
            end
            % Close the waitbar
            delete(hWaitbar);
        end

        % Placeholder function for DisconnectSerialPort
        function DisconnectSerialPort(app)
                try
                    % Check if the serial connection exists and is open
                    if ~isempty(app.SerialConnection) && isvalid(app.SerialConnection)
                        fclose(app.SerialConnection);
                        delete(app.SerialConnection);
                        clear app.SerialConnection;
                        uialert(app.UIFigure, 'Disconnected successfully', 'Disconnection');
                    end
                catch e
                    uialert(app.UIFigure, ['Error disconnecting: ' e.message], 'Disconnection Error');
                end
            end
        end

    %% App initialization and construction - wielkosc/pozycja okna i przyciskow czyli wyglad UI
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)
            % Get screen size
            screenSize = get(groot, 'ScreenSize');
            screenWidth = screenSize(3);
            screenHeight = screenSize(4);
        
            % Set the size of the UIFigure
            figureWidth = 640;
            figureHeight = 480;

             % Calculate the position to center the figure
            positionX = (screenWidth - figureWidth) / 2;
            positionY = (screenHeight - figureHeight) / 2;

            % Create UIFigure
            app.UIFigure = uifigure;
            app.UIFigure.Resize = 'off';
            app.UIFigure.Position = [positionX, positionY, figureWidth, figureHeight]; % Adjust the size as needed
            app.UIFigure.Name = 'Temperature Control System';

            % Create StartButton
            app.StartButton = uibutton(app.UIFigure, 'push');
            app.StartButton.Position = [50, 420, 100, 22];
            app.StartButton.Text = 'Start';
            app.StartButton.ButtonPushedFcn = createCallbackFcn(app, @StartButtonPushed, true);

            % Create PauseButton
            app.PauseButton = uibutton(app.UIFigure, 'push');
            app.PauseButton.Position = [160, 420, 100, 22];
            app.PauseButton.Text = 'Pause';
            app.PauseButton.ButtonPushedFcn = createCallbackFcn(app, @PauseButtonPushed, true);

            % Create ResumeButton
            app.ResumeButton = uibutton(app.UIFigure, 'push');
            app.ResumeButton.Position = [270, 420, 100, 22];
            app.ResumeButton.Text = 'Resume';
            app.ResumeButton.ButtonPushedFcn = createCallbackFcn(app, @ResumeButtonPushed, true);

            % Create SaveButton
            app.SaveButton = uibutton(app.UIFigure, 'push');
            app.SaveButton.Position = [380, 420, 100, 22];
            app.SaveButton.Text = 'Save';
            app.SaveButton.ButtonPushedFcn = createCallbackFcn(app, @SaveButtonPushed, true);

            % Create ConnectButton
            app.ConnectButton = uibutton(app.UIFigure, 'push');
            app.ConnectButton.Position = [490, 420, 100, 22];
            app.ConnectButton.Text = 'Connect';
            app.ConnectButton.ButtonPushedFcn = createCallbackFcn(app, @ConnectButtonPushed, true);

            % Create SetTempSlider
            app.SetTempSlider = uislider(app.UIFigure);
            app.SetTempSlider.Position = [50, 60, 540, 3];
            app.SetTempSlider.ValueChangedFcn = createCallbackFcn(app, @SetTempSliderValueChanged, true);

            % Create CurrentTempLabel
            app.CurrentTempLabel = uilabel(app.UIFigure);
            app.CurrentTempLabel.Position = [50, 380, 100, 22];
            app.CurrentTempLabel.Text = 'Current Temp';
            
            % Create CurrentTemp_read
            app.CurrentTemp_data = uilabel(app.UIFigure);
            app.CurrentTemp_data.Position = [150, 380, 100, 22];
            app.CurrentTemp_data.Text = '';

            % Create SetTempLabel
            app.SetTempLabel = uilabel(app.UIFigure);
            app.SetTempLabel.Position = [50, 340, 100, 22];
            app.SetTempLabel.Text = 'Set Temp';

            % Create TempTrendAxes
            app.TempTrendAxes = uiaxes(app.UIFigure);
            app.TempTrendAxes.Position = [50, 100, 540, 220]; % Adjust the size as needed
            app.TempTrendAxes.XLabel.String = 'Time';
            app.TempTrendAxes.YLabel.String = 'Temperature';

            % Create SetTempEditField
            app.SetTempEditField = uieditfield(app.UIFigure, 'text');
            app.SetTempEditField.Position = [150, 340, 100, 22];
            app.SetTempEditField.ValueChangedFcn = createCallbackFcn(app, @SetTempEditFieldValueChanged, true);

            % Create SetButton
            app.SetButton = uibutton(app.UIFigure, 'push');
            app.SetButton.Position = [260, 340, 100, 22];
            app.SetButton.Text = 'Ustaw';
            app.SetButton.ButtonPushedFcn = createCallbackFcn(app, @SetButtonPushed, true);

            % Create SimulateButton
            app.SimulateButton = uibutton(app.UIFigure, 'push');
            app.SimulateButton.Position = [370, 340, 100, 22];
            app.SimulateButton.Text = 'Zasymuluj';
            app.SimulateButton.ButtonPushedFcn = createCallbackFcn(app, @SimulateButtonPushed, true);
        
        end
    end

    % App creation and deletion
    methods (Access = public)

        % Construct app
        function app = GUIAPP
            % Create and configure components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.UIFigure)

            % Run the startup function
            runStartupFcn(app, @startupFcn)

            % Show the figure after all components are created
            app.UIFigure.Visible = 'on';
        end
    end
end
