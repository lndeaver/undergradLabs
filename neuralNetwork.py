#Lauren Deaver
#lndeaver
from numpy import loadtxt
from keras.models import Sequential
from keras.layers import Dense
from keras.utils import to_categorical
from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import LabelEncoder

def get_model():
	#load dataset
	dataset=loadtxt('wine_train.csv',delimiter=",")
	#split into input (x) and output (y)
	X=dataset[:,1:]
	y=dataset[:,0]
	#encode class as ints
	encoder = LabelEncoder()
	encoder.fit(y)
	encoded_y = encoder.transform(y)
	#make ints into dummy vars
	#makeeveryhting have to be in same range
	#treat equal
	dummy_y = to_categorical(encoded_y)
	dummy_X = MinMaxScaler().fit_transform(X, dummy_y)

	MinMaxScaler().fit_transform(dummy_y)
	#define keras mdoel
	model=Sequential()
	model.add(Dense(12,input_dim=13,activation='relu'))
	model.add(Dense(8,activation='relu'))
	model.add(Dense(3,activation='softmax'))
	#compile model
	model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
	#fit model
	model.fit(dummy_X,dummy_y,epochs=150,batch_size=10,verbose=0)
	#see accuracy and predictions
	_, accuracy=model.evaluate(dummy_X,dummy_y,verbose=0)
	print('Accuracy: %.2f' % (accuracy*100))

	predictions = model.predict_classes(X)
	print(predictions)

	return model

get_model()
